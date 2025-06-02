include sys/peripheral/Makefile.mk
include sys/startup/Makefile.mk

CFLAGS 		+= -I./sys
CPPFLAGS 	+= -I./sys

CFLAGS 		+= -I./sys
CPPFLAGS 	+= -I./sys

VPATH += ./sys

# C source files
SOURCES += ./sys/sys_startup.c
SOURCES += ./sys/sys_irq.c
SOURCES += ./sys/sys_cfg.c
