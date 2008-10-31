MOTO_GUI = 'moto-gui'

CC  = 'gcc'
CXX = 'g++'

CFLAGS  = ['-O2', '-pipe', '-ffast-math', '-Wall', '-march=i686']
LIBS    = ['GL', 'GLU', 'GLEW']

CPPFLAGS = ['-DMOTO_WOBJ_MESH_LOADER',
            # '-DMOTO_MBM_MESH_LOADER',
            # '-DMOTO_RIB_MESH_LOADER',
            ]

PKG_CONFIG = 'pkg-config gtk+-2.0 gtkglext-1.0 gio-2.0 gthread-2.0 --cflags --libs'

PLUGINS = []
