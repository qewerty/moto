/* ##################################################################################
#
#  Moto Animation System (http://motoanim.sf.net)
#  Copyleft (C) 2008 Konstantin Evdokimenko a.k.a Qew[erty] (qewerty@gmail.com)
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License
#  as published by the Free Software Foundation; either version 2
#  of the License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#
################################################################################## */

#ifndef __MOTO_NODE_H__
#define __MOTO_NODE_H__

#include <glib-object.h>

#include "moto-forward.h"

G_BEGIN_DECLS

typedef void (*MotoNodeActionFunc)(MotoNode *node);
typedef void (*MotoNodeForeachParamFunc)(MotoNode *node, MotoParam *param, gpointer user_data);
typedef void (*MotoNodeForeachGroupFunc)(MotoNode *node, const gchar *group, gpointer user_data);
typedef void (*MotoNodeForeachParamInGroupFunc)(MotoNode *node, const gchar *group, MotoParam *param, gpointer user_data);

typedef enum
{
    MOTO_PARAM_MODE_IN      = 1,
    MOTO_PARAM_MODE_OUT     = 2,
    MOTO_PARAM_MODE_INOUT   = 3
} MotoParamMode; // TODO: Rename to MOTO_PARAM_DIRECTION_*

GType moto_param_mode_get_type(void);
#define MOTO_TYPE_PARAM_MODE (moto_param_mode_get_type())

/* class MotoNode */

struct _MotoNode
{
    GObject parent;
};

struct _MotoNodeClass
{
    GObjectClass parent;

    GData *actions;

    /* Virtual Table */
    MotoNodeUpdateMethod update;

    /* Signals */
    guint source_changed_signal_id;
};

GType moto_node_get_type(void);

#define MOTO_TYPE_NODE (moto_node_get_type())
#define MOTO_NODE(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_NODE, MotoNode))
#define MOTO_NODE_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_NODE, MotoNodeClass))
#define MOTO_IS_NODE(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_NODE))
#define MOTO_IS_NODE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_NODE))
#define MOTO_NODE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_NODE, MotoNodeClass))

MotoNode *moto_create_node(GType type, const gchar *name);
MotoNode *moto_create_node_by_name(const gchar *type_name, const gchar *name);

void moto_node_do_action(MotoNode *self, const gchar *action_name);
void moto_node_set_action(MotoNode *self, const gchar *action_name, MotoNodeActionFunc func);

const gchar *moto_node_get_name(MotoNode *self);
void moto_node_set_name(MotoNode *self, const gchar *name);

guint moto_node_get_id(MotoNode *self);

gboolean moto_node_is_valid(MotoNode *self);

void moto_node_add_dynamic_param(MotoNode *self, MotoParam *param,
        const gchar *domain, const gchar *group);
#define moto_node_add_param(self, param, domain, group) \
    moto_node_add_dynamic_param(self, param, domain, group)
void moto_node_add_params(MotoNode *self, ...);
MotoParam *moto_node_get_param(MotoNode *self, const gchar *name);
GValue *moto_node_get_param_value(MotoNode *self, const gchar *name);

gboolean moto_node_get_param_boolean(MotoNode *self,    const gchar *name);
gint     moto_node_get_param_int(MotoNode *self,        const gchar *name);
guint    moto_node_get_param_uint(MotoNode *self,       const gchar *name);
glong    moto_node_get_param_long(MotoNode *self,       const gchar *name);
gulong   moto_node_get_param_ulong(MotoNode *self,      const gchar *name);
gint64   moto_node_get_param_int64(MotoNode *self,      const gchar *name);
guint64  moto_node_get_param_uint64(MotoNode *self,     const gchar *name);
gfloat   moto_node_get_param_float(MotoNode *self,      const gchar *name);
gdouble  moto_node_get_param_double(MotoNode *self,     const gchar *name);
const gchar *moto_node_get_param_string(MotoNode *self,    const gchar *name);
gpointer moto_node_get_param_pointer(MotoNode *self,    const gchar *name);
gint     moto_node_get_param_enum(MotoNode *self,       const gchar *name);
GObject *moto_node_get_param_object(MotoNode *self,     const gchar *name);

void moto_node_set_param_boolean(MotoNode *self,    const gchar *name, gboolean value);
void moto_node_set_param_int(MotoNode *self,        const gchar *name, gint     value);
void moto_node_set_param_uint(MotoNode *self,       const gchar *name, guint    value);
void moto_node_set_param_long(MotoNode *self,       const gchar *name, glong    value);
void moto_node_set_param_ulong(MotoNode *self,      const gchar *name, gulong   value);
void moto_node_set_param_int64(MotoNode *self,      const gchar *name, gint64   value);
void moto_node_set_param_uint64(MotoNode *self,     const gchar *name, guint64  value);
void moto_node_set_param_float(MotoNode *self,      const gchar *name, gfloat   value);
void moto_node_set_param_double(MotoNode *self,     const gchar *name, gdouble  value);
void moto_node_set_param_pointer(MotoNode *self,    const gchar *name, gpointer value);
void moto_node_set_param_enum(MotoNode *self,       const gchar *name, gint     value);
void moto_node_set_param_object(MotoNode *self,     const gchar *name, GObject *value);

// boolean
void moto_node_set_param_1b(MotoNode *self, const gchar *name, gboolean v);
void moto_node_set_param_1bv(MotoNode *self, const gchar *name, const gboolean *v);

void moto_node_set_param_2b(MotoNode *self,  const gchar *name, gboolean v0, gboolean v1);
void moto_node_set_param_2bv(MotoNode *self, const gchar *name, const gboolean *v);

void moto_node_set_param_3b(MotoNode *self,  const gchar *name, gboolean v0, gboolean v1, gboolean v2);
void moto_node_set_param_3bv(MotoNode *self, const gchar *name, const gboolean *v);

void moto_node_set_param_4b(MotoNode *self,  const gchar *name, gboolean v0, gboolean v1, gboolean v2, gboolean v3);
void moto_node_set_param_4bv(MotoNode *self, const gchar *name, const gboolean *v);

void moto_node_set_param_Nbv(MotoNode *self, const gchar *name, const gboolean *v, gsize N);

// int
void moto_node_set_param_1i(MotoNode *self, const gchar *name, gint v);
void moto_node_set_param_1iv(MotoNode *self, const gchar *name, const gint *v);

void moto_node_set_param_2i(MotoNode *self,  const gchar *name, gint v0, gint v1);
void moto_node_set_param_2iv(MotoNode *self, const gchar *name, const gint *v);

void moto_node_set_param_3i(MotoNode *self,  const gchar *name, gint v0, gint v1, gint v2);
void moto_node_set_param_3iv(MotoNode *self, const gchar *name, const gint *v);

void moto_node_set_param_4i(MotoNode *self,  const gchar *name, gint v0, gint v1, gint v2, gint v3);
void moto_node_set_param_4iv(MotoNode *self, const gchar *name, const gint *v);

void moto_node_set_param_Niv(MotoNode *self, const gchar *name, const gint *v, gsize N);

// uint
void moto_node_set_param_1ui(MotoNode *self, const gchar *name, guint v);
void moto_node_set_param_1uiv(MotoNode *self, const gchar *name, const guint *v);

void moto_node_set_param_2ui(MotoNode *self,  const gchar *name, guint v0, guint v1);
void moto_node_set_param_2uiv(MotoNode *self, const gchar *name, const guint *v);

void moto_node_set_param_3ui(MotoNode *self,  const gchar *name, guint v0, guint v1, guint v2);
void moto_node_set_param_3uiv(MotoNode *self, const gchar *name, const guint *v);

void moto_node_set_param_4ui(MotoNode *self,  const gchar *name, guint v0, guint v1, guint v2, guint v3);
void moto_node_set_param_4uiv(MotoNode *self, const gchar *name, const guint *v);

void moto_node_set_param_Nuiv(MotoNode *self, const gchar *name, const guint *v, gsize N);

// long
void moto_node_set_param_1l(MotoNode *self, const gchar *name, glong v);
void moto_node_set_param_1lv(MotoNode *self, const gchar *name, const glong *v);

void moto_node_set_param_2l(MotoNode *self,  const gchar *name, glong v0, glong v1);
void moto_node_set_param_2lv(MotoNode *self, const gchar *name, const glong *v);

void moto_node_set_param_3l(MotoNode *self,  const gchar *name, glong v0, glong v1, glong v2);
void moto_node_set_param_3lv(MotoNode *self, const gchar *name, const glong *v);

void moto_node_set_param_4l(MotoNode *self,  const gchar *name, glong v0, glong v1, glong v2, glong v3);
void moto_node_set_param_4lv(MotoNode *self, const gchar *name, const glong *v);

void moto_node_set_param_Nlv(MotoNode *self, const gchar *name, const glong *v, gsize N);

// ulong
void moto_node_set_param_1ul(MotoNode *self, const gchar *name, gulong v);
void moto_node_set_param_1ulv(MotoNode *self, const gchar *name, const gulong *v);

void moto_node_set_param_2ul(MotoNode *self,  const gchar *name, gulong v0, gulong v1);
void moto_node_set_param_2ulv(MotoNode *self, const gchar *name, const gulong *v);

void moto_node_set_param_3ul(MotoNode *self,  const gchar *name, gulong v0, gulong v1, gulong v2);
void moto_node_set_param_3ulv(MotoNode *self, const gchar *name, const gulong *v);

void moto_node_set_param_4ul(MotoNode *self,  const gchar *name, gulong v0, gulong v1, gulong v2, gulong v3);
void moto_node_set_param_4ulv(MotoNode *self, const gchar *name, const gulong *v);

void moto_node_set_param_Nulv(MotoNode *self, const gchar *name, const gulong *v, gsize N);

// int64
void moto_node_set_param_1i64(MotoNode *self, const gchar *name, gint64 v);
void moto_node_set_param_1i64v(MotoNode *self, const gchar *name, const gint64 *v);

void moto_node_set_param_2i64(MotoNode *self,  const gchar *name, gint64 v0, gint64 v1);
void moto_node_set_param_2i64v(MotoNode *self, const gchar *name, const gint64 *v);

void moto_node_set_param_3i64(MotoNode *self,  const gchar *name, gint64 v0, gint64 v1, gint64 v2);
void moto_node_set_param_3i64v(MotoNode *self, const gchar *name, const gint64 *v);

void moto_node_set_param_4i64(MotoNode *self,  const gchar *name, gint64 v0, gint64 v1, gint64 v2, gint64 v3);
void moto_node_set_param_4i64v(MotoNode *self, const gchar *name, const gint64 *v);

void moto_node_set_param_Ni64v(MotoNode *self, const gchar *name, const gint64 *v, gsize N);

// uint64
void moto_node_set_param_1ui64(MotoNode *self, const gchar *name, guint64 v);
void moto_node_set_param_1ui64v(MotoNode *self, const gchar *name, const guint64 *v);

void moto_node_set_param_2ui64(MotoNode *self,  const gchar *name, guint64 v0, guint64 v1);
void moto_node_set_param_2ui64v(MotoNode *self, const gchar *name, const guint64 *v);

void moto_node_set_param_3ui64(MotoNode *self,  const gchar *name, guint64 v0, guint64 v1, guint64 v2);
void moto_node_set_param_3ui64v(MotoNode *self, const gchar *name, const guint64 *v);

void moto_node_set_param_4ui64(MotoNode *self,  const gchar *name, guint64 v0, guint64 v1, guint64 v2, guint64 v3);
void moto_node_set_param_4ui64v(MotoNode *self, const gchar *name, const guint64 *v);

void moto_node_set_param_Nui64v(MotoNode *self, const gchar *name, const guint64 *v, gsize N);

// float
void moto_node_set_param_1f(MotoNode *self, const gchar *name, gfloat v);
void moto_node_set_param_1fv(MotoNode *self, const gchar *name, const gfloat *v);

void moto_node_set_param_2f(MotoNode *self,  const gchar *name, gfloat v0, gfloat v1);
void moto_node_set_param_2fv(MotoNode *self, const gchar *name, const gfloat *v);

void moto_node_set_param_3f(MotoNode *self,  const gchar *name, gfloat v0, gfloat v1, gfloat v2);
void moto_node_set_param_3fv(MotoNode *self, const gchar *name, const gfloat *v);

void moto_node_set_param_4f(MotoNode *self,  const gchar *name, gfloat v0, gfloat v1, gfloat v2, gfloat v3);
void moto_node_set_param_4fv(MotoNode *self, const gchar *name, const gfloat *v);

void moto_node_set_param_Nfv(MotoNode *self, const gchar *name, const gfloat *v, gsize N);

// float
void moto_node_set_param_1d(MotoNode *self, const gchar *name, gdouble v);
void moto_node_set_param_1dv(MotoNode *self, const gchar *name, const gdouble *v);

void moto_node_set_param_2d(MotoNode *self,  const gchar *name, gdouble v0, gdouble v1);
void moto_node_set_param_2dv(MotoNode *self, const gchar *name, const gdouble *v);

void moto_node_set_param_3d(MotoNode *self,  const gchar *name, gdouble v0, gdouble v1, gdouble v2);
void moto_node_set_param_3dv(MotoNode *self, const gchar *name, const gdouble *v);

void moto_node_set_param_4d(MotoNode *self,  const gchar *name, gdouble v0, gdouble v1, gdouble v2, gdouble v3);
void moto_node_set_param_4dv(MotoNode *self, const gchar *name, const gdouble *v);

void moto_node_set_param_Ndv(MotoNode *self, const gchar *name, const gdouble *v, gsize N);

void moto_node_get_params(MotoNode *self, ...);
void moto_node_set_params(MotoNode *self, ...);

void moto_node_foreach_param(MotoNode *self,
        MotoNodeForeachParamFunc func, gpointer user_data);

void moto_node_foreach_group(MotoNode *self,
        MotoNodeForeachGroupFunc func, gpointer user_data);
void moto_node_foreach_param_in_group(MotoNode *self, const gchar *group_name,
        MotoNodeForeachParamInGroupFunc func, gpointer user_data);

void moto_node_link(MotoNode *self, const gchar *in_name,
                    MotoNode *other, const gchar *out_name);

#define moto_node_param_value_pointer(self, name, c_type) \
    moto_param_value_pointer(moto_node_get_param(self, name), c_type)

void moto_node_update_param_dests(MotoNode *self, const gchar *name);
void moto_node_update_all_param_dests(MotoNode *self);

void moto_node_save_to_variation(MotoNode *self, MotoVariation *variation);
void moto_node_restore_from_variation(MotoNode *self, MotoVariation *variation);

gboolean moto_node_is_hidden(MotoNode *self);
void moto_node_set_hidden(MotoNode *self, gboolean hidden);
void moto_node_hide(MotoNode *self);
void moto_node_show(MotoNode *self);

const GTimeVal *moto_node_get_last_modified(MotoNode *self);
void moto_node_update_last_modified(MotoNode *self);

/* Get manipulator wth given name or NULL if not found. */
MotoManipulator *moto_node_get_manipulator(MotoNode *self, const gchar *name);
/* Get first manipulator in the list or NULL if node has no manipulators. */
MotoManipulator *moto_node_get_first_manipulator(MotoNode *self);
/* Get next manipulator in the list.
 * If manip argument is the last manipulator or it's not in the list or manip is NULL get first. */
MotoManipulator *moto_node_get_next_manipulator(MotoNode *self, MotoManipulator *manip);

void moto_node_set_tag(MotoNode *self, const gchar *tag);
void moto_node_del_tag(MotoNode *self, const gchar *tag); 
gboolean moto_node_has_tag(MotoNode *self, const gchar *tag);

/* Get dump for saving. */
gconstpointer moto_node_get_dump(MotoNode *self, glong *numbytes);

/* Update node internals. This must not affect on other nodes. */
void moto_node_update(MotoNode *self);

/* Just moto_node_update if there is no cache or restore state of node from undo/redo cache. */
void moto_node_undo(MotoNode *self);
void moto_node_redo(MotoNode *self);

MotoWorld *moto_node_get_world(MotoNode *self);
void moto_node_set_world(MotoNode *self, MotoWorld *world);

MotoLibrary *moto_node_get_library(MotoNode *self);

gboolean moto_node_depends_on(MotoNode *self, MotoNode *other);

/* Returns TRUE if node changes during animation or FALSE otherwise.
 * Used for optimization. */
gboolean moto_node_is_animated(MotoNode *self);

/* class MotoParam */

struct _MotoParam
{
    GObject parent;
};

struct _MotoParamClass
{
    GObjectClass parent;

    guint value_changed_signal_id;
    guint source_changed_signal_id;
    guint dest_added_signal_id;
    guint dest_removed_signal_id;
};

GType moto_param_get_type(void);

#define MOTO_TYPE_PARAM (moto_param_get_type())
#define MOTO_PARAM(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_PARAM, MotoParam))
#define MOTO_PARAM_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_PARAM, MotoParamClass))
#define MOTO_IS_PARAM(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_PARAM))
#define MOTO_IS_PARAM_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_PARAM))
#define MOTO_PARAM_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_PARAM, MotoParamClass))

MotoParam *moto_param_new(const gchar *name, const gchar *title,
        MotoParamMode mode, GValue *value, GParamSpec *pspec,
        MotoNode *node);

const gchar *moto_param_get_name(MotoParam *self);
const gchar *moto_param_get_title(MotoParam *self);
MotoNode *moto_param_get_node(MotoParam *self);
MotoParamMode moto_param_get_mode(MotoParam *self);

GParamSpec *moto_param_get_spec(MotoParam *self);

guint moto_param_get_id(MotoParam *self);

GValue * moto_param_get_value(MotoParam *self);
gpointer moto_param_get_value_pointer(MotoParam *self);
#define moto_param_value_pointer(p, c_type) ((c_type *)moto_param_get_value_pointer(p))
#define moto_param_value(p, c_type) (*moto_param_value_pointer(p, c_type))

GType moto_param_get_value_type(MotoParam *self);

gboolean moto_param_get_boolean(MotoParam *self);
gint     moto_param_get_int(MotoParam *self);
guint    moto_param_get_uint(MotoParam *self);
glong    moto_param_get_long(MotoParam *self);
gulong   moto_param_get_ulong(MotoParam *self);
gint64   moto_param_get_int64(MotoParam *self);
guint64  moto_param_get_uint64(MotoParam *self);
gfloat   moto_param_get_float(MotoParam *self);
gdouble  moto_param_get_double(MotoParam *self);
const gchar *moto_param_get_string(MotoParam *self);
gpointer moto_param_get_pointer(MotoParam *self);
gint     moto_param_get_enum(MotoParam *self);
GObject *moto_param_get_object(MotoParam *self);

// Set parameter value
void moto_param_set_boolean(MotoParam *self,    gboolean    value);
void moto_param_set_int(MotoParam *self,        gint        value);
void moto_param_set_uint(MotoParam *self,       guint       value);
void moto_param_set_long(MotoParam *self,       glong       value);
void moto_param_set_ulong(MotoParam *self,      gulong      value);
void moto_param_set_int64(MotoParam *self,      gint64      value);
void moto_param_set_uint64(MotoParam *self,     guint64     value);
void moto_param_set_float(MotoParam *self,      gfloat      value);
void moto_param_set_double(MotoParam *self,     gdouble     value);
void moto_param_set_string(MotoParam *self,     const gchar *value);
void moto_param_set_pointer(MotoParam *self,    gpointer    value);
void moto_param_set_enum(MotoParam *self,       gint        value);
void moto_param_set_object(MotoParam *self,     GObject    *value);

// boolean
void moto_param_set_1b(MotoParam *self, gboolean v);
void moto_param_set_1bv(MotoParam *self, const gboolean *v);

void moto_param_set_2b(MotoParam *self, gboolean v0, gboolean v1);
void moto_param_set_2bv(MotoParam *self, const gboolean *v);

void moto_param_set_3b(MotoParam *self, gboolean v0, gboolean v1, gboolean v2);
void moto_param_set_3bv(MotoParam *self, const gboolean *v);

void moto_param_set_4b(MotoParam *self, gboolean v0, gboolean v1, gboolean v2, gboolean v3);
void moto_param_set_4bv(MotoParam *self, const gboolean *v);

void moto_param_set_Nbv(MotoParam *self, const gchar *name, const gboolean *v, gsize N);

// int
void moto_param_set_1i(MotoParam *self, gint v);
void moto_param_set_1iv(MotoParam *self, const gint *v);

void moto_param_set_2i(MotoParam *self, gint v0, gint v1);
void moto_param_set_2iv(MotoParam *self, const gint *v);

void moto_param_set_3i(MotoParam *self, gint v0, gint v1, gint v2);
void moto_param_set_3iv(MotoParam *self, const gint *v);

void moto_param_set_4i(MotoParam *self, gint v0, gint v1, gint v2, gint v3);
void moto_param_set_4iv(MotoParam *self, const gint *v);

void moto_param_set_Niv(MotoParam *self, const gchar *name, const gint *v, gsize N);

// uint
void moto_param_set_1ui(MotoParam *self, guint v);
void moto_param_set_1uiv(MotoParam *self, const guint *v);

void moto_param_set_2ui(MotoParam *self, guint v0, guint v1);
void moto_param_set_2uiv(MotoParam *self, const guint *v);

void moto_param_set_3ui(MotoParam *self, guint v0, guint v1, guint v2);
void moto_param_set_3uiv(MotoParam *self, const guint *v);

void moto_param_set_4ui(MotoParam *self, guint v0, guint v1, guint v2, guint v3);
void moto_param_set_4uiv(MotoParam *self, const guint *v);

void moto_param_set_Nuiv(MotoParam *self, const gchar *name, const guint *v, gsize N);

// long
void moto_param_set_1l(MotoParam *self, glong v);
void moto_param_set_1lv(MotoParam *self, const glong *v);

void moto_param_set_2l(MotoParam *self, glong v0, glong v1);
void moto_param_set_2lv(MotoParam *self, const glong *v);

void moto_param_set_3l(MotoParam *self, glong v0, glong v1, glong v2);
void moto_param_set_3lv(MotoParam *self, const glong *v);

void moto_param_set_4l(MotoParam *self, glong v0, glong v1, glong v2, glong v3);
void moto_param_set_4lv(MotoParam *self, const glong *v);

void moto_param_set_Nlv(MotoParam *self, const gchar *name, const glong *v, gsize N);

// ulong
void moto_param_set_1ul(MotoParam *self, gulong v);
void moto_param_set_1ulv(MotoParam *self, const gulong *v);

void moto_param_set_2ul(MotoParam *self, gulong v0, gulong v1);
void moto_param_set_2ulv(MotoParam *self, const gulong *v);

void moto_param_set_3ul(MotoParam *self, gulong v0, gulong v1, gulong v2);
void moto_param_set_3ulv(MotoParam *self, const gulong *v);

void moto_param_set_4ul(MotoParam *self, gulong v0, gulong v1, gulong v2, gulong v3);
void moto_param_set_4ulv(MotoParam *self, const gulong *v);

void moto_param_set_Nulv(MotoParam *self, const gchar *name, const gulong *v, gsize N);

// int64
void moto_param_set_1i64(MotoParam *self, gint64 v);
void moto_param_set_1i64v(MotoParam *self, const gint64 *v);

void moto_param_set_2i64(MotoParam *self, gint64 v0, gint64 v1);
void moto_param_set_2i64v(MotoParam *self, const gint64 *v);

void moto_param_set_3i64(MotoParam *self, gint64 v0, gint64 v1, gint64 v2);
void moto_param_set_3i64v(MotoParam *self, const gint64 *v);

void moto_param_set_4i64(MotoParam *self, gint64 v0, gint64 v1, gint64 v2, gint64 v3);
void moto_param_set_4i64v(MotoParam *self, const gint64 *v);

void moto_param_set_Ni64v(MotoParam *self, const gchar *name, const gint64 *v, gsize N);

// uint64
void moto_param_set_1ui64(MotoParam *self, guint64 v);
void moto_param_set_1ui64v(MotoParam *self, const guint64 *v);

void moto_param_set_2ui64(MotoParam *self, guint64 v0, guint64 v1);
void moto_param_set_2ui64v(MotoParam *self, const guint64 *v);

void moto_param_set_3ui64(MotoParam *self, guint64 v0, guint64 v1, guint64 v2);
void moto_param_set_3ui64v(MotoParam *self, const guint64 *v);

void moto_param_set_4ui64(MotoParam *self, guint64 v0, guint64 v1, guint64 v2, guint64 v3);
void moto_param_set_4ui64v(MotoParam *self, const guint64 *v);

void moto_param_set_Nui64v(MotoParam *self, const gchar *name, const guint64 *v, gsize N);

// float
void moto_param_set_1f(MotoParam *self, gfloat v);
void moto_param_set_1fv(MotoParam *self, const gfloat *v);

void moto_param_set_2f(MotoParam *self, gfloat v0, gfloat v1);
void moto_param_set_2fv(MotoParam *self, const gfloat *v);

void moto_param_set_3f(MotoParam *self, gfloat v0, gfloat v1, gfloat v2);
void moto_param_set_3fv(MotoParam *self, const gfloat *v);

void moto_param_set_4f(MotoParam *self, gfloat v0, gfloat v1, gfloat v2, gfloat v3);
void moto_param_set_4fv(MotoParam *self, const gfloat *v);

void moto_param_set_Nfv(MotoParam *self, const gchar *name, const gfloat *v, gsize N);

// double
void moto_param_set_1d(MotoParam *self, gdouble v);
void moto_param_set_1dv(MotoParam *self, const gdouble *v);

void moto_param_set_a1d(MotoParam *self, glong start, glong num, const gdouble *v);

void moto_param_set_2d(MotoParam *self, gdouble v0, gdouble v1);
void moto_param_set_2dv(MotoParam *self, const gdouble *v);

void moto_param_set_a2d(MotoParam *self, glong start, glong num, const gdouble *v);

void moto_param_set_3d(MotoParam *self, gdouble v0, gdouble v1, gdouble v2);
void moto_param_set_3dv(MotoParam *self, const gdouble *v);

void moto_param_set_a3d(MotoParam *self, glong start, glong num, const gdouble *v);

void moto_param_set_4d(MotoParam *self, gdouble v0, gdouble v1, gdouble v2, gdouble v3);
void moto_param_set_4dv(MotoParam *self, const gdouble *v);

void moto_param_set_a4d(MotoParam *self, glong start, glong num, const gdouble *v);

void moto_param_set_Ndv(MotoParam *self, const gchar *name, const gdouble *v, gsize N);

void moto_param_set_aNd(MotoParam *self, glong start, glong num, const gdouble *v, gsize N);

void moto_param_set_2x2dr(MotoParam *self, gdouble v00, gdouble v01,
                                          gdouble v10, gdouble v11);
void moto_param_set_2x2drv(MotoParam *self, const gdouble *v);
void moto_param_set_2x2dc(MotoParam *self, gdouble v00, gdouble v01,
                                          gdouble v10, gdouble v11);
void moto_param_set_2x2dcv(MotoParam *self, const gdouble *v);

void moto_param_set_3x3d(MotoParam *self, gdouble v00, gdouble v01, gdouble v02,
                                          gdouble v10, gdouble v11, gdouble v12,
                                          gdouble v20, gdouble v21, gdouble v22);
void moto_param_set_3x3dv(MotoParam *self, const gdouble *v);

void moto_param_set_4x4d(MotoParam *self, gdouble v00, gdouble v01, gdouble v02, gdouble v03,
                                          gdouble v10, gdouble v11, gdouble v12, gdouble v13,
                                          gdouble v20, gdouble v21, gdouble v22, gdouble v23,
                                          gdouble v30, gdouble v31, gdouble v32, gdouble v33);
void moto_param_set_4x4dv(MotoParam *self, const gdouble *v);

void moto_param_set_a4x4dv(MotoParam *self, const gdouble *v, ...);
void moto_param_set_ae4x4dv(MotoParam *self, const gdouble *v, ...);

gboolean moto_param_is_array(MotoParam *self);
gboolean moto_param_is_array_unsized(MotoParam *self);

// Variability can hols following values:
//      MOTO_PARAM_VARIABILITY_CONSTANT, // Not animated
//      MOTO_PARAM_VARIABILITY_UNIFORM,  // Animated, value changes from frame to frame
//      MOTO_PARAM_VARIABILITY_VARYING   // Animated, value can change from frame to frame
//                                       // and also in one frame from component to component
// void moto_param_set_variability(MotoParam *self), MotoParamVariability variability);
// MotoParamVariability moto_param_get_variability(MotoParam *self);

// Only for unsized arrays
void moto_param_set_array_size(MotoParam *self, gsize size);
void moto_param_set_array_multi_dim_size(MotoParam *self, gsize *sizes);

gboolean moto_param_is_struct(MotoParam *self);

// Returns base type of array and matrix
GType moto_param_get_base_type(MotoParam *self);

// Retuns the number of rows. Same as moto_param_get_dim_size(param, 0)
gint moto_param_get_rows(MotoParam *self);
// Retuns the number of columns. Same as moto_param_get_dim_size(param, 1)
gint moto_param_get_cols(MotoParam *self);
// Returns the number of dimensions
gint moto_param_get_dims(MotoParam *self);
// Returns the number of elements in the given dimension
gint moto_param_get_dim_size(MotoParam *self, gint dimension);
// Returns the total number of elements
gint moto_param_get_total_size(MotoParam *self);

void moto_param_set_use_expression(MotoParam *self, gboolean use);
gboolean moto_param_get_use_expression(MotoParam *self);

void moto_param_set_expression(MotoParam *self, const gchar *code);
const gchar *moto_param_get_expression(MotoParam *self);
gboolean moto_param_eval(MotoParam *self);

void moto_param_update(MotoParam *self);
void moto_param_update_dests(MotoParam *self);

/* Valid only if mode is IN or INOUT and does nothing if else. */
MotoParam *moto_param_get_source(MotoParam *self);
void moto_param_link(MotoParam *self, MotoParam *src);
void moto_param_unlink_source(MotoParam *self);

/* Valid only if mode is OUT or INOUT and does nothing if else. */
void moto_param_unlink_dests(MotoParam *self);

/* Just calls moto_param_unlink_source and moto_param_unlink_dests. */
void moto_param_unlink(MotoParam *self);

gboolean moto_param_has_dests(MotoParam *self);

/* May be FALSE if source is invalid or when limits are exceeded.  */
gboolean moto_param_is_valid(MotoParam *self);

gboolean moto_param_is_animated(MotoParam *self);

G_END_DECLS

#endif /* __MOTO_NODE_H__ */
