#ifdef _WIN32
    #include <windows.h>
#else
    #define GLX_GLXEXT_LEGACY
#endif

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

#ifdef _WIN32
    #include <GL/wglew.h>
#else
    #include <GL/glxew.h>
#endif

#include <glib.h>

void moto_gl_print_list(void);
void moto_gl_print_info(void);
void moto_gl_init(void);
gboolean moto_gl_is_supported(const gchar *name);

gboolean moto_gl_is_glsl_supported();
gboolean moto_gl_is_vbo_supported();
