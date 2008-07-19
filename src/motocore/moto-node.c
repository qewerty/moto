#include "common/moto-mapped-list.h"

#include "moto-node.h"
#include "moto-world.h"
#include "moto-param-data.h"
#include "moto-messager.h"

/* utils */

static void
unref_gobject(gpointer data, gpointer user_data)
{
    g_object_unref(data);
}

/* forwards */

static GType moto_node_factory_get_node_type_virtual(MotoNodeFactory *self);

/* privates */

struct _MotoNodePriv
{
    gboolean disposed;

    GString *name;
    MotoWorld *world;

    MotoMappedList params;
    MotoMappedList pdomains;

    gboolean hidden;

    GSList *tags;
    GSList *notes;

    /* For exporting optimization. */
    GTimeVal last_modified;
};

struct _MotoNodeFactoryPriv
{
    gboolean disposed;

    GType node_type;
};

struct _MotoParamPriv
{
    gboolean disposed;

    GValue value;
    GValue default_value;
    MotoParam *source;
    GSList *dests;

    GString *name;
    GString *title;
    MotoNode *node;
    MotoParamMode mode;

    gboolean hidden;
    GTimeVal last_modified; /* For exporting optimization. */

    /* GList *notes; // ??? */
};

/* class Node */

static GObjectClass *node_parent_class = NULL;

static void
moto_node_dispose(GObject *obj)
{
    MotoNode *self = (MotoNode *)obj;

    if(self->priv->disposed)
        return;
    self->priv->disposed = TRUE;

    g_string_free(self->priv->name, TRUE);
    moto_mapped_list_free_all(& self->priv->params, unref_gobject);
    // moto_mapped_list_free_all(& self->priv->pdomains, unref_gobject);

    node_parent_class->dispose(obj);
}

static void
moto_node_finalize(GObject *obj)
{
    MotoNode *self = (MotoNode *)obj;

    g_slice_free(MotoNodePriv, self->priv);

    node_parent_class->finalize(obj);
}

static void
moto_node_init(MotoNode *self)
{
    self->priv = g_slice_new(MotoNodePriv);
    self->priv->disposed = FALSE;

    self->priv->name = g_string_new("");
    self->priv->world = NULL;

    moto_mapped_list_init(& self->priv->params);
    moto_mapped_list_init(& self->priv->pdomains);

    self->priv->hidden = FALSE;
    g_get_current_time(& self->priv->last_modified);

    self->priv->tags = NULL;
}

static void
moto_node_class_init(MotoNodeClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;

    node_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose    = moto_node_dispose;
    goclass->finalize   = moto_node_finalize;

    klass->update = NULL;
}

G_DEFINE_ABSTRACT_TYPE(MotoNode, moto_node, G_TYPE_OBJECT);

/* methods of class Node */

const gchar *moto_node_get_name(MotoNode *self)
{
    return self->priv->name->str;
}

void moto_node_set_name(MotoNode *self, const gchar *name)
{
    g_string_assign(self->priv->name, name);
}

MotoParam *moto_node_get_param(MotoNode *self, const gchar *name)
{
    // TODO
}

gboolean moto_node_is_hidden(MotoNode *self)
{
    return self->priv->hidden;
}

void moto_node_set_hidden(MotoNode *self, gboolean hidden)
{
    self->priv->hidden = hidden;
}

void moto_node_hide(MotoNode *self)
{
    moto_node_set_hidden(self, FALSE);
}

void moto_node_show(MotoNode *self)
{
    moto_node_set_hidden(self, TRUE);
}

void moto_node_update(MotoNode *self)
{
    MotoNodeClass *klass = MOTO_NODE_GET_CLASS(self);

    if(klass->update)
        klass->update(self);
}

const GTimeVal *moto_node_get_last_modified(MotoNode *self)
{
    return & self->priv->last_modified;
}

void moto_node_update_last_modified(MotoNode *self)
{
    g_get_current_time(& self->priv->last_modified);
}

void moto_node_set_tag(MotoNode *self, const gchar *tag)
{
    GSList *curtag = self->priv->tags;
    for(; curtag; curtag = g_slist_next(tag))
    {
        if(g_utf8_collate(((GString *)curtag->data)->str, tag) == 0)
            return;
    }

    GString *str = g_string_new(tag);
    self->priv->tags = g_slist_append(self->priv->tags, str);
}

void moto_node_del_tag(MotoNode *self, const gchar *tag)
{
    GSList *curtag = self->priv->tags;
    for(; curtag; curtag = g_slist_next(tag))
    {
        if(g_utf8_collate((gchar *)(curtag->data), tag) == 0)
        {
            self->priv->tags = g_slist_delete_link(self->priv->tags, curtag);
            g_string_free((GString *)curtag->data, TRUE);
        }
    }
}

gboolean moto_node_has_tag(MotoNode *self, const gchar *tag)
{
    GSList *curtag = self->priv->tags;
    for(; curtag; curtag = g_slist_next(tag))
    {
        if(g_utf8_collate(((GString *)curtag->data)->str, tag) == 0)
            return TRUE;
    }
    return FALSE;
}

MotoWorld *moto_node_get_world(MotoNode *self)
{
    return self->priv->world;
}

void moto_node_set_world(MotoNode *self, MotoWorld *world)
{
    self->priv->world = world;
}

MotoLibrary *moto_node_get_library(MotoNode *self)
{
    MotoWorld *w = moto_node_get_world(self);
    if( ! w)
        return NULL;
    return moto_world_get_library(w);
}

/* class NodeFactory */

static GObjectClass *node_factory_parent_class = NULL;

static void
moto_node_factory_dispose(GObject *obj)
{
    /* MotoNodeFactory *self = (MotoNodeFactory *)obj; */
    node_factory_parent_class->dispose(obj);
}

static void
moto_node_factory_finalize(GObject *obj)
{
    node_factory_parent_class->finalize(obj);
}

static void
moto_node_factory_init(MotoNodeFactory *self)
{
    self->priv = (MotoNodeFactoryPriv *)g_slice_new(MotoNodeFactoryPriv);

    self->priv->node_type = G_TYPE_INVALID;
}

static void
moto_node_factory_class_init(MotoNodeFactoryClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;

    node_factory_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose    = moto_node_factory_dispose;
    goclass->finalize   = moto_node_factory_finalize;

    klass->get_node_type    = moto_node_factory_get_node_type_virtual;
    klass->create_node      = NULL;

    klass->create_node_signal_id = g_signal_newv ("create-node",
                 G_TYPE_FROM_CLASS (klass),
                 G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                 NULL /* class closure */,
                 NULL /* accumulator */,
                 NULL /* accu_data */,
                 g_cclosure_marshal_VOID__VOID,
                 G_TYPE_NONE /* return_type */,
                 0     /* n_params */,
                 NULL  /* param_types */);

    klass->node_created_signal_id = g_signal_newv ("node-created",
                 G_TYPE_FROM_CLASS (klass),
                 G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                 NULL /* class closure */,
                 NULL /* accumulator */,
                 NULL /* accu_data */,
                 g_cclosure_marshal_VOID__VOID,
                 G_TYPE_NONE /* return_type */,
                 0     /* n_params */,
                 NULL  /* param_types */);

}

G_DEFINE_ABSTRACT_TYPE(MotoNodeFactory, moto_node_factory, G_TYPE_OBJECT);

MotoNode *
moto_node_factory_create_node(MotoNodeFactory *self, const gchar *name)
{
    MotoNodeFactoryClass *klass = MOTO_NODE_FACTORY_GET_CLASS(self);

    g_signal_emit(self, klass->create_node_signal_id, 0, NULL);

    if(G_TYPE_IS_ABSTRACT(self->priv->node_type))
    {
        GString *msg = g_string_new("You are trying to create instance of abstract node type (\"");
        g_string_append(msg, g_type_name(self->priv->node_type));
        g_string_append(msg, "\"). I won't create it.");
        moto_warning(msg->str);
        g_string_free(msg, TRUE);
        return NULL;
    }

    MotoNode *node = NULL;

    if(klass->create_node)
        node = klass->create_node(self, name);

    g_signal_emit(self, klass->node_created_signal_id, 0, NULL);

    return node;
}

GType moto_node_factory_get_node_type(MotoNodeFactory *self)
{
    MotoNodeFactoryClass *klass = MOTO_NODE_FACTORY_GET_CLASS(self);

    if(klass->get_node_type)
        return klass->get_node_type(self);
    return G_TYPE_INVALID;
}

static GType moto_node_factory_get_node_type_virtual(MotoNodeFactory *self)
{
    return MOTO_TYPE_NODE;
}

/* class Param */

static GObjectClass *param_parent_class = NULL;

static void
moto_param_dispose(GObject *obj)
{
    MotoParam *self = (MotoParam *)obj;

    if(self->priv->disposed)
        return;
    self->priv->disposed = TRUE;

    g_object_unref(self->priv->data);
    g_string_free(self->priv->name, TRUE);
    g_string_free(self->priv->title, TRUE);

    param_parent_class->dispose(obj);
}

static void
moto_param_finalize(GObject *obj)
{
    MotoParam *self = (MotoParam *)obj;

    g_slice_free(MotoParamPriv, self->priv);

    G_OBJECT_CLASS(param_parent_class)->finalize(obj);
}

static void
moto_param_init(MotoParam *self)
{
    self->priv = (MotoParamPriv *)g_slice_new(MotoParamPriv);
    self->priv->disposed = FALSE;

    self->priv->data = NULL;

    self->priv->source = NULL;
    self->priv->dests = NULL;

    self->priv->name = g_string_new("");
    self->priv->title = g_string_new("");

    self->priv->mode = MOTO_PARAM_MODE_IN;

    self->priv->hidden = FALSE;
    g_get_current_time(& self->priv->last_modified);

    self->priv->pb = NULL;
}

static void
moto_param_class_init(MotoParamClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;

    param_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose    = moto_param_dispose;
    goclass->finalize   = moto_param_finalize;
}

G_DEFINE_TYPE(MotoParam, moto_param, G_TYPE_OBJECT);

/* methods of class Param */

MotoParam *moto_param_new(const gchar *name, const gchar *title,
        MotoNode *node, MotoParamMode mode, GValue *value)
{
    MotoParam *self = (MotoParam *)g_object_new(MOTO_TYPE_PARAM, NULL);

    return self;
}

void moto_param_set_from_string(MotoParam *self,
        const gchar *string)
{

}

const gchar *moto_param_get_name(MotoParam *self)
{
    return self->priv->name->str;
}

const gchar *moto_param_get_title(MotoParam *self)
{
    return self->priv->title->str;
}

MotoParamMode moto_param_get_mode(MotoParam *self)
{
    return self->priv->mode;
}

static void param_setup_ptr(MotoParam *self, MotoParam *src)
{
    moto_param_data_point(self->priv->data, moto_param_data_get(src->priv->data));
}

MotoParam *moto_param_get_source(MotoParam *self)
{
    return self->priv->source;
}

static void
disconnect_on_source_deleted(gpointer data, GObject *where_the_object_was)
{
    MotoParam *param = (MotoParam *)data;

    param->priv->source = NULL;
}

static void
exclude_from_dests_on_dest_deleted(gpointer data, GObject *where_the_object_was)
{
    MotoParam *param = (MotoParam *)data;

    param->priv->dests = g_slist_remove(param->priv->dests, where_the_object_was);
}

void moto_param_set_source(MotoParam *self, MotoParam *src)
{
    if( ! src)
    {
        GString *msg = g_string_new("You are trying to connect nothing (None|NULL). I won't connect it.");
        moto_warning(msg->str);
        g_string_free(msg, TRUE);
        return;
    }

    if(src->priv->mode == MOTO_PARAM_MODE_IN)
    {
        GString *msg = g_string_new("You are trying to connect source that has no output (\"");
        g_string_append(msg, moto_param_get_name(self));
        g_string_append(msg, "\"). I won't connect it.");
        moto_warning(msg->str);
        g_string_free(msg, TRUE);
        return;
    }

    if(self->priv->mode == MOTO_PARAM_MODE_OUT)
    {
        GString *msg = g_string_new("You are trying to connect source to output parameter (\"");
        g_string_append(msg, moto_param_get_name(self));
        g_string_append(msg, "\"). I won't connect it.");
        moto_warning(msg->str);
        g_string_free(msg, TRUE);
        return;
    }

    /* TODO: Type checking! */

    if(src == self->priv->source)
        return;

    g_object_weak_ref(G_OBJECT(src), disconnect_on_source_deleted, self);
    g_object_weak_ref(G_OBJECT(self), exclude_from_dests_on_dest_deleted, src);

    self->priv->source = src;
    src->priv->dests = g_slist_append(src->priv->dests, self);

    param_setup_ptr(self, src);
}

void moto_param_clear_source(MotoParam *self)
{

    if(self->priv->source)
    {
        g_object_weak_unref(G_OBJECT(self->priv->source), disconnect_on_source_deleted, self);
        g_object_weak_unref(G_OBJECT(self), exclude_from_dests_on_dest_deleted, self->priv->source);

        self->priv->source->priv->dests = g_slist_remove(self->priv->source->priv->dests, self);
    }
    self->priv->source = NULL;
}

static void
null_source(gpointer data, gpointer user_data)
{
    ((MotoParam *)data)->priv->source = NULL;
}

void moto_param_clear_dests(MotoParam *self)
{
    if(self->priv->mode == MOTO_PARAM_MODE_IN)
    {
        GString *msg = g_string_new("You are trying to clear destinations of input parameter (\"");
        g_string_append(msg, moto_param_get_name(self));
        g_string_append(msg, "\"). Inputs may not have destinations.");
        moto_warning(msg->str);
        g_string_free(msg, TRUE);
        return;
    }

    g_slist_foreach(self->priv->dests, null_source, NULL);
    g_slist_free(self->priv->dests);
    self->priv->dests = NULL;
}

gboolean moto_param_has_dests(MotoParam *self)
{
    return (g_slist_length(self->priv->dests) > 0);
}

gboolean moto_param_is_valid(MotoParam *self)
{
    return (MOTO_IS_PARAM_BLOCK(self->priv->pb) && \
            MOTO_IS_NODE(moto_param_block_get_node(self->priv->pb)));
}

MotoNode *moto_param_get_node(MotoParam *self)
{
    if(self->priv->pb == NULL)
        return NULL;

    return moto_param_block_get_node(self->priv->pb);
}

void moto_param_update(MotoParam *self)
{
    if(self->priv->data)
        moto_param_data_update(self->priv->data);
}

void moto_param_update_dests(MotoParam *self)
{
    if(self->priv->mode == MOTO_PARAM_MODE_IN)
        return;

    GSList *dest = self->priv->dests;
    for(; dest; dest = g_slist_next(dest))
    {
        MotoParam *param = (MotoParam *)dest->data;
        moto_param_update(param);
    }

}
