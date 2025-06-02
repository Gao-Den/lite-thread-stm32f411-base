include sys/peripheral/peripheral_libraries/Makefile.mk

CFLAGS 		+= -I./sys/peripheral
CPPFLAGS 	+= -I./sys/peripheral

CFLAGS 		+= -I./sys/peripheral
CPPFLAGS 	+= -I./sys/peripheral

VPATH += ./sys/peripheral

# C source files
SOURCES += ./sys/peripheral/io_cfg.c
