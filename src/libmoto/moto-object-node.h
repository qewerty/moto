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

#ifndef __MOTO_OBJECT_NODE_H__
#define __MOTO_OBJECT_NODE_H__

#include <GL/glew.h>
#include <GL/glu.h>

#include "moto-ray.h"
#include "moto-transform-info.h"
#include "moto-node.h"
#include "moto-bound.h"
#include "moto-shape-node.h"

G_BEGIN_DECLS

typedef struct _MotoObjectNodePriv MotoObjectNodePriv;

typedef const MotoBound *(*MotoObjectNodeGetBoundMethod)(MotoObjectNode *self);

typedef enum
{
    MOTO_TRANSFORM_ORDER_TRS,
    MOTO_TRANSFORM_ORDER_TSR,
    MOTO_TRANSFORM_ORDER_RTS,
    MOTO_TRANSFORM_ORDER_RST,
    MOTO_TRANSFORM_ORDER_STR,
    MOTO_TRANSFORM_ORDER_SRT
} MotoTransformOrder;

GType moto_transform_order_get_type(void);
#define MOTO_TYPE_TRANSFORM_ORDER (moto_transform_order_get_type())

typedef enum
{
    MOTO_ROTATE_ORDER_XYZ,
    MOTO_ROTATE_ORDER_XZY,
    MOTO_ROTATE_ORDER_YXZ,
    MOTO_ROTATE_ORDER_YZX,
    MOTO_ROTATE_ORDER_ZXY,
    MOTO_ROTATE_ORDER_ZYX
} MotoRotateOrder;

GType moto_rotate_order_get_type(void);
#define MOTO_TYPE_ROTATE_ORDER (moto_rotate_order_get_type())

/* class MotoObjectNode */

struct _MotoObjectNode
{
    MotoNode parent;

    MotoObjectNodePriv *priv;
};

struct _MotoObjectNodeClass
{
    MotoNodeClass parent;

    /* signals */
    guint button_press_signal_id;
    guint button_release_signal_id;
    guint motion_notify_signal_id;
};

GType moto_object_node_get_type(void);

#define MOTO_TYPE_OBJECT_NODE (moto_object_node_get_type())
#define MOTO_OBJECT_NODE(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_OBJECT_NODE, MotoObjectNode))
#define MOTO_OBJECT_NODE_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_OBJECT_NODE, MotoObjectNodeClass))
#define MOTO_IS_OBJECT_NODE(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_OBJECT_NODE))
#define MOTO_IS_OBJECT_NODE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_OBJECT_NODE))
#define MOTO_OBJECT_NODE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_OBJECT_NODE, MotoObjectNodeClass))

MotoObjectNode *moto_object_node_new(const gchar *name);

MotoBound *moto_object_node_get_bound(MotoObjectNode *self, gboolean global);
void moto_object_node_draw_full(MotoObjectNode *self,
        gboolean recursive, gboolean use_global);
void moto_object_node_draw(MotoObjectNode* self);

MotoDrawMode moto_object_node_get_draw_mode(MotoObjectNode* self);

MotoShapeNode* moto_object_node_get_shape(MotoObjectNode* self);

MotoShapeSelection* moto_object_node_get_selection(MotoObjectNode* self);
MotoSelectionMode moto_object_node_get_selection_mode(MotoObjectNode* self);
void moto_object_node_set_selection_mode(MotoObjectNode* self, MotoSelectionMode mode);

void moto_object_node_select_more(MotoObjectNode* self);
void moto_object_node_select_less(MotoObjectNode* self);
void moto_object_node_select_inverse(MotoObjectNode* self);

void moto_object_node_look_at(MotoObjectNode *self, gfloat eye[3], gfloat look[3], gfloat up[3]);
void moto_object_node_slide(MotoObjectNode *self,
        gfloat dx, gfloat dy, gfloat dz);
void moto_object_node_zoom(MotoObjectNode *self, gfloat val);
void moto_object_node_tumble_h(MotoObjectNode *self, gfloat da);
void moto_object_node_tumble_v(MotoObjectNode *self, gfloat da);
void moto_object_node_roll(MotoObjectNode *self, gfloat da);
void moto_object_node_pitch(MotoObjectNode *self, gfloat da);
void moto_object_node_yaw(MotoObjectNode *self, gfloat da);
void moto_object_node_apply_camera_transform(MotoObjectNode *self, gint width, gint height);
void moto_object_node_set_rotate_order(MotoObjectNode *self, MotoRotateOrder order);

void moto_object_node_set_translate(MotoObjectNode *self, gfloat x, gfloat y, gfloat z);

gfloat *moto_object_node_get_matrix(MotoObjectNode *self, gboolean global);
gfloat *moto_object_node_get_inverse_matrix(MotoObjectNode *self, gboolean global);

gboolean moto_object_node_button_press(MotoObjectNode *self,
    gint x, gint y, gint width, gint height, MotoRay *ray,
    MotoTransformInfo *tinfo);
gboolean moto_object_node_button_release(MotoObjectNode *self,
    gint x, gint y, gint width, gint height);
gboolean moto_object_node_motion(MotoObjectNode *self,
    gint x, gint y, gint width, gint height);

G_END_DECLS

#endif /* __MOTO_OBJECT_NODE_H__ */

