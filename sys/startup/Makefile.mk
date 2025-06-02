CFLAGS 		+= -I./sys/startup
CPPFLAGS 	+= -I./sys/startup

CFLAGS 		+= -I./sys/startup
CPPFLAGS 	+= -I./sys/startup

VPATH += ./sys/startup

# C source files
SOURCES += ./sys/startup/sys_startup.c

# CPP source files
SOURCES_CPP += ./sys/startup/mini_cpp.cpp
