#include "stdlib.h"
#include "math.h"

#include <gtk/gtkgl.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "moto-test-window.h"
#include "motocore/moto-world.h"
#include "motocore/moto-system.h"
#include "motocore/moto-node.h"
#include "motocore/moto-object-node.h"
#include "motocore/moto-param-data.h"
#include "motocore/moto-float-param-data.h"
#include "common/numdef.h"

/* forwards */

static void init_gl(GtkWidget *widget, gpointer data);
static gboolean draw(GtkWidget *widget, GdkEventExpose *event, gpointer data);

static gboolean
reshape(GtkWidget *widget, GdkEventConfigure *event, gpointer data);

static gboolean
press_mouse_button(GtkWidget *widget, GdkEventButton *event, gpointer data);
static gboolean
release_mouse_button(GtkWidget *widget, GdkEventButton *event, gpointer data);
static gboolean
mouse_motion(GtkWidget *widget, GdkEventMotion *event, gpointer data);

/* class TestWindow */

static GObjectClass *test_window_parent_class = NULL;

struct _MotoTestWindowPriv
{
    MotoSystem *system;
    MotoWorld *world;

    GtkDrawingArea *area;

    gboolean disposed;
};

static void
moto_test_window_dispose(GObject *obj)
{
    MotoTestWindow *self = (MotoTestWindow *)obj;

    if(self->priv->disposed)
        return;
    self->priv->disposed = TRUE;

    g_object_unref(self->priv->system);

    test_window_parent_class->dispose(obj);
}

static void
moto_test_window_finalize(GObject *obj)
{
    MotoTestWindow *self = (MotoTestWindow *)obj;
    g_slice_free(MotoTestWindowPriv, self->priv);

    test_window_parent_class->finalize(obj);
}

static void
moto_test_window_init(MotoTestWindow *self)
{
    self->priv = g_slice_new(MotoTestWindowPriv);
    self->priv->disposed = FALSE;

    self->priv->system = moto_system_new();
    moto_system_get_library(self->priv->system);
    self->priv->world = moto_world_new("My Test World", moto_system_get_library(self->priv->system));
    moto_system_add_world(self->priv->system, self->priv->world, TRUE);

    MotoNode *root_node = moto_world_create_node(self->priv->world, "MotoObjectNode", "Root");
    moto_world_set_root(self->priv->world, (MotoObjectNode *)root_node);

    MotoNode *obj_node = moto_world_create_node(self->priv->world, "MotoObjectNode", "Object");
    MotoParam *in = moto_node_get_param(obj_node, "main", "parent");
    MotoParam *out = moto_node_get_param(root_node, "main", "transform");
    moto_param_set_source(in, out);

    MotoNode *grid_view_node = moto_world_create_node(self->priv->world, "MotoGridViewNode", "GridView");

    in = moto_node_get_param(obj_node, "view", "view");
    out = moto_node_get_param(grid_view_node, "main", "view");
    moto_param_set_source(in, out);

    obj_node = moto_world_create_node(self->priv->world, "MotoObjectNode", "Object");
    in = moto_node_get_param(obj_node, "main", "parent");
    out = moto_node_get_param(root_node, "main", "transform");
    moto_param_set_source(in, out);

    moto_world_set_axes(self->priv->world, obj_node);

    MotoNode *axes_view_node = moto_world_create_node(self->priv->world, "MotoAxesViewNode", "AxesView");

    in = moto_node_get_param(obj_node, "view", "view");
    out = moto_node_get_param(axes_view_node, "main", "view");
    moto_param_set_source(in, out);

    obj_node = moto_world_create_node(self->priv->world, "MotoObjectNode", "Object");
    in = moto_node_get_param(obj_node, "main", "parent");
    out = moto_node_get_param(root_node, "main", "transform");
    moto_param_set_source(in, out);

    MotoNode *view_node = moto_world_create_node(self->priv->world, "MotoMeshViewNode", "View");
    MotoNode *cube_node = moto_world_create_node(self->priv->world, "MotoCubeNode", "Cube");
    MotoNode *mat_node = moto_world_create_node(self->priv->world, "MotoSlerMaterialNode", "Material");

    in = moto_node_get_param(obj_node, "view", "view");
    out = moto_node_get_param(view_node, "main", "view");
    moto_param_set_source(in, out);

    in = moto_node_get_param(view_node, "main", "mesh");
    out = moto_node_get_param(cube_node, "main", "mesh");
    moto_param_set_source(in, out);

    in = moto_node_get_param(obj_node, "shading", "material");
    out = moto_node_get_param(mat_node, "main", "material");
    moto_param_set_source(in, out);

    /* camera */
    MotoNode *cam_obj = moto_world_create_node(self->priv->world, "MotoObjectNode", "CameraObject");
    // MotoNode *cam = moto_world_create_node(self->priv->world, "MotoCameraNode", "CameraObject");
    moto_world_set_camera(self->priv->world, (MotoObjectNode *)cam_obj);
    MotoParam *param = moto_node_get_param(cam_obj, "main", "tx");
    MotoFloatParamData *tx = (MotoFloatParamData *)moto_param_get_data(param);
    param = moto_node_get_param(cam_obj, "main", "ty");
    MotoFloatParamData *ty = (MotoFloatParamData *)moto_param_get_data(param);
    param = moto_node_get_param(cam_obj, "main", "tz");
    MotoFloatParamData *tz = (MotoFloatParamData *)moto_param_get_data(param);
    param = moto_node_get_param(cam_obj, "main", "rx");
    MotoFloatParamData *rx = (MotoFloatParamData *)moto_param_get_data(param);
    param = moto_node_get_param(cam_obj, "main", "ry");
    MotoFloatParamData *ry = (MotoFloatParamData *)moto_param_get_data(param);
    param = moto_node_get_param(cam_obj, "main", "rz");
    MotoFloatParamData *rz = (MotoFloatParamData *)moto_param_get_data(param);

    moto_float_param_data_set(tx, 0);
    moto_float_param_data_set(ty, 0);
    moto_float_param_data_set(tz, 0);
    /* moto_float_param_data_set(rx, RAD_PER_DEG*-45);
    moto_float_param_data_set(ry, RAD_PER_DEG*5);
    moto_float_param_data_set(rz, RAD_PER_DEG*23); */

    self->priv->area = (GtkDrawingArea *)gtk_drawing_area_new();
    GtkWidget *area = (GtkWidget *)self->priv->area;
    GdkGLConfig *gl_config =\
                gdk_gl_config_new_by_mode((GdkGLConfigMode)(GDK_GL_MODE_RGBA | GDK_GL_MODE_DOUBLE | GDK_GL_MODE_DEPTH));
        if (gl_config == NULL) {
                g_print("\nNUll: Error - Unable to initialize OpenGL!\n");
                exit(1);
        }
    gtk_widget_set_gl_capability(area, gl_config,
                NULL, TRUE, GDK_GL_RGBA_TYPE);

    gtk_widget_add_events(GTK_WIDGET(area), GDK_BUTTON1_MOTION_MASK | GDK_BUTTON2_MOTION_MASK |
            GDK_BUTTON_PRESS_MASK | GDK_VISIBILITY_NOTIFY_MASK | GDK_BUTTON_RELEASE_MASK |
            GDK_POINTER_MOTION_MASK | GDK_SCROLL_MASK);

    gtk_container_add(GTK_CONTAINER(self), area);
    g_signal_connect(G_OBJECT(self), "delete-event",
                G_CALLBACK(gtk_main_quit), NULL);

    g_signal_connect(G_OBJECT(area), "expose-event",
                G_CALLBACK(draw), NULL);
    g_signal_connect(G_OBJECT(area), "realize",
                G_CALLBACK(init_gl), NULL);
    g_signal_connect(G_OBJECT(area), "configure-event",
                G_CALLBACK(reshape), NULL);

    g_signal_connect(G_OBJECT(area), "motion-notify-event",
            G_CALLBACK(mouse_motion), NULL);

    g_signal_connect(G_OBJECT(area), "button-press-event",
        G_CALLBACK(press_mouse_button), NULL);
    g_signal_connect(G_OBJECT(area), "button-release-event",
        G_CALLBACK(release_mouse_button), NULL);
}

static void
moto_test_window_class_init(MotoTestWindowClass *klass)
{
    GObjectClass *goclass = G_OBJECT_CLASS(klass);

    test_window_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose    = moto_test_window_dispose;
    goclass->finalize   = moto_test_window_finalize;
}

G_DEFINE_TYPE(MotoTestWindow, moto_test_window, GTK_TYPE_WINDOW);

/* methods of class TestWindow */

GtkWindow *moto_test_window_new()
{
    GtkWindow *self = (GtkWindow *)g_object_new(MOTO_TYPE_TEST_WINDOW, NULL);
    // MotoTestWindow *twin = (MotoTestWindow *)self;

    // MotoWorld *w = twin->priv->world;

    return self;
}

static void init_gl(GtkWidget *widget, gpointer data)
{
    GdkGLContext *gl_context = gtk_widget_get_gl_context(widget);
    GdkGLDrawable *gl_drawable = gtk_widget_get_gl_drawable(widget);

    if(!GDK_IS_GL_DRAWABLE(gl_drawable)) return;
    if(!gdk_gl_drawable_gl_begin(gl_drawable, gl_context)) return;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

    glClearColor(0.2, 0.2, 0.2, 1);

    gdk_gl_drawable_gl_end(gl_drawable);
}

static gboolean
draw(GtkWidget *widget,
     GdkEventExpose *event,
     gpointer data)
{
    if(!GTK_WIDGET_REALIZED(widget)) return FALSE;

    GdkGLContext *gl_context = gtk_widget_get_gl_context(widget);
    GdkGLDrawable *gl_drawable = gtk_widget_get_gl_drawable(widget);
    if(!GDK_IS_GL_DRAWABLE(gl_drawable)) return FALSE;
    if(!gdk_gl_drawable_gl_begin(gl_drawable, gl_context)) return FALSE;

    MotoTestWindow *twin = (MotoTestWindow *)gtk_widget_get_parent(widget);

    gint width = widget->allocation.width;
    gint height = widget->allocation.height;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glColor3f(0.7, 0.7, 0.7);

    moto_world_draw(twin->priv->world, width, height);

    gdk_gl_drawable_swap_buffers(gl_drawable);

    return TRUE;
}

static gboolean
reshape(GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
    GdkGLContext *gl_context = gtk_widget_get_gl_context(widget);
    GdkGLDrawable *gl_drawable = gtk_widget_get_gl_drawable(widget);

    if( ! GDK_IS_GL_DRAWABLE(gl_drawable)) return FALSE;
    if( ! gdk_gl_drawable_gl_begin(gl_drawable, gl_context)) return FALSE;

    gdk_gl_drawable_gl_end(gl_drawable);

    return FALSE;
}

static gboolean move = FALSE;
static gboolean zoom = FALSE;
static gboolean roll = FALSE;
static gdouble prev_x = 0;
static gdouble prev_y = 0;
MotoRotateOrder ro = MOTO_ROTATE_ORDER_XYZ;

static gboolean
press_mouse_button(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    if(!GTK_WIDGET_REALIZED(widget)) return FALSE;

    MotoTestWindow *twin = (MotoTestWindow *)gtk_widget_get_parent(widget);
    MotoWorld *world = twin->priv->world;
    MotoObjectNode *cam = moto_world_get_camera(world);

    gint width = widget->allocation.width;
    gint height = widget->allocation.height;

    prev_x = event->x;
    prev_y = event->y;

    switch(event->button)
    {
        case 1:
            if(event->state & GDK_MOD1_MASK)
                roll = TRUE;

            if(event->state & GDK_CONTROL_MASK)
            {
                if(ro == MOTO_ROTATE_ORDER_XYZ)
                    ro = MOTO_ROTATE_ORDER_YZX;
                else
                    ro = MOTO_ROTATE_ORDER_XYZ;
                moto_object_node_set_rotate_order(cam, ro);
                draw(widget, (GdkEventExpose *)event, data);
            }
        break;
        case 2:
            if(event->state & GDK_MOD1_MASK)
                move = TRUE;
        break;
        case 3:
            if(event->state & GDK_MOD1_MASK)
                zoom = TRUE;
        break;
    }

    draw(widget, (GdkEventExpose *)event, data);
    return TRUE;
}

static gboolean
release_mouse_button(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    if(!GTK_WIDGET_REALIZED(widget)) return FALSE;

    move = FALSE;
    zoom = FALSE;
    roll = FALSE;

    if(event->button == 2)
    {

    }

    draw(widget, (GdkEventExpose *)event, data);
    return TRUE;
}

static gboolean
mouse_motion(GtkWidget *widget, GdkEventMotion *event, gpointer data)
{
    GdkGLContext *gl_context = gtk_widget_get_gl_context(widget);
    GdkGLDrawable *gl_drawable = gtk_widget_get_gl_drawable(widget);

    gint width = widget->allocation.width;
    gint height = widget->allocation.height;

    // temp
    if( (! prev_x) && (! prev_y))
    {
        prev_x = event->x;
        prev_y = event->y;
    }

    if(!GDK_IS_GL_DRAWABLE(gl_drawable)) return TRUE;

    if (!gdk_gl_drawable_gl_begin(gl_drawable, gl_context)) return FALSE;

    MotoTestWindow *twin = (MotoTestWindow *)gtk_widget_get_parent(widget);
    MotoWorld *world = twin->priv->world;
    MotoObjectNode *cam = moto_world_get_camera(world);

    if(cam && move)
    {
        moto_object_node_slide(cam,
                (prev_x - event->x) / width * 15,
                (event->y - prev_y) / height * 15,
                0);
    }
    else if(cam && zoom)
    {
        gfloat xx = prev_x - event->x;
        gfloat yy = prev_y - event->y;
        gfloat factor = xx;

        if(fabs(yy) > fabs(xx))
            factor = yy;

        moto_object_node_zoom(cam, factor*0.025);
        // moto_object_node_slide(cam, 0, 0, factor*0.025);
    }
    else if(cam && roll)
    {
        gfloat xx = prev_x - event->x;
        gfloat yy = prev_y - event->y;

        if(event->state & GDK_SHIFT_MASK)
        {
            gfloat factor = xx;

            if(fabs(yy) > fabs(xx))
            {
                factor  = yy;
            }
            gint sign = (factor>0)?1:-1;
            moto_object_node_roll(cam, 1*sign);
        }
        else
        {
            // moto_object_node_tumble(cam, yy*0.2, xx*0.2);
            moto_object_node_tumble_h(cam, yy*0.2);
            moto_object_node_tumble_v(cam, xx*0.2);
        }
    }
    else
        return TRUE;

    draw(widget, (GdkEventExpose *)event, data);

    prev_x = event->x;
    prev_y = event->y;

    return TRUE;
}
