#include <math.h>

#include "moto-types.h"
#include "moto-param-spec.h"
#include "moto-mesh.h"
#include "moto-extrude-node.h"
#include "libmotoutil/moto-gl.h"
#include "libmotoutil/matrix.h"
#include "libmotoutil/numdef.h"

/* forwards */

static void moto_extrude_node_update(MotoNode *self);

/* class MotoExtrudeNode */

typedef struct _MotoExtrudeNodePriv MotoExtrudeNodePriv;
#define MOTO_EXTRUDE_NODE_GET_PRIVATE(obj) G_TYPE_INSTANCE_GET_PRIVATE(obj, MOTO_TYPE_EXTRUDE_NODE, MotoExtrudeNodePriv)

static GObjectClass *normal_move_node_parent_class = NULL;

struct _MotoExtrudeNodePriv
{
    gint dummy;
};

static void
moto_extrude_node_dispose(GObject *obj)
{
    normal_move_node_parent_class->dispose(obj);
}

static void
moto_extrude_node_finalize(GObject *obj)
{
    normal_move_node_parent_class->finalize(obj);
}

static void
moto_extrude_node_init(MotoExtrudeNode *self)
{
    MotoNode *node = (MotoNode *)self;
    MotoExtrudeNodePriv *priv = MOTO_EXTRUDE_NODE_GET_PRIVATE(self);

    /* params */

    MotoParamSpec *sections_spec = moto_param_spec_int_new(1, 1, 100000, 1, 2);
    moto_node_add_params(node,
            "sections",  "Sections",   G_TYPE_INT,   MOTO_PARAM_MODE_INOUT, 1, sections_spec, "Arguments",
            "length",    "Length",     MOTO_TYPE_FLOAT, MOTO_PARAM_MODE_INOUT, 0.5f, NULL,          "Arguments",
            "in_mesh",  "Input Mesh",  MOTO_TYPE_MESH,  MOTO_PARAM_MODE_IN,    NULL, NULL,          "Geometry",
            "out_mesh", "Output Mesh", MOTO_TYPE_MESH,  MOTO_PARAM_MODE_OUT,   NULL, NULL,          "Geometry",
            NULL);

    g_object_unref(sections_spec);
}

static void
moto_extrude_node_class_init(MotoExtrudeNodeClass *klass)
{
    g_type_class_add_private(klass, sizeof(MotoExtrudeNodePriv));

    normal_move_node_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    GObjectClass *goclass = G_OBJECT_CLASS(klass);
    MotoNodeClass *nclass = (MotoNodeClass *)klass;

    goclass->dispose    = moto_extrude_node_dispose;
    goclass->finalize   = moto_extrude_node_finalize;

    nclass->update = moto_extrude_node_update;
}

G_DEFINE_TYPE(MotoExtrudeNode, moto_extrude_node, MOTO_TYPE_NODE);

/* Methods of class MotoExtrudeNode */

MotoExtrudeNode *moto_extrude_node_new(const gchar *name)
{
    MotoExtrudeNode *self = (MotoExtrudeNode *)g_object_new(MOTO_TYPE_EXTRUDE_NODE, NULL);
    MotoNode *node = (MotoNode *)self;

    moto_node_set_name(node, name);

    return self;
}

static void moto_extrude_node_update(MotoNode *self)
{
    MotoExtrudeNodePriv *priv = MOTO_EXTRUDE_NODE_GET_PRIVATE(self);

    MotoMesh *in_mesh;
    moto_node_get_param_object(self, "in_mesh", (GObject**)&in_mesh);
    if( ! in_mesh)
    {
        moto_node_set_param_object(self, "out_mesh", NULL);
        MotoParam *param = moto_node_get_param((MotoNode *)self, "out_mesh");
        moto_param_update_dests(param);
        return;
    }

    gint sections;
    moto_node_get_param_int(self, "sections", &sections);
    if(sections < 1)
    {
        moto_node_set_param_object(self, "out_mesh", NULL);
        MotoParam *param = moto_node_get_param((MotoNode *)self, "out_mesh");
        moto_param_update_dests(param);
        return;
    }

    gfloat length;
    moto_node_get_param_float(self, "length", &length);

    MotoMeshSelection *selection = moto_mesh_selection_new_for_mesh(in_mesh);
    moto_mesh_selection_select_faces(selection, 4, G_MAXUINT32);
    MotoMesh *mesh = moto_mesh_extrude_faces(in_mesh, selection, sections, length);

    moto_mesh_selection_free(selection);

    MotoMesh *old_mesh;
    moto_node_get_param_object((MotoNode *)self, "out_mesh", (GObject**)&old_mesh);
    if(old_mesh)
        g_object_unref(old_mesh);

    MotoParam *param = moto_node_get_param((MotoNode *)self, "out_mesh");
    moto_param_set_object(param, (GObject *)mesh);
    moto_param_update_dests(param);
}

