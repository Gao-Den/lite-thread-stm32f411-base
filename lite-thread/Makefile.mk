CFLAGS 		+= -I./lite-thread/inc
CPPFLAGS 	+= -I./lite-thread/inc
CFLAGS 		+= -I./lite-thread/common
CPPFLAGS 	+= -I./lite-thread/common
CFLAGS 		+= -I./lite-thread/common/console
CPPFLAGS 	+= -I./lite-thread/common/console
CFLAGS 		+= -I./lite-thread/common/container
CPPFLAGS 	+= -I./lite-thread/common/container
CFLAGS 		+= -I./lite-thread/common/log
CPPFLAGS 	+= -I./lite-thread/common/log

VPATH += ./lite-thread/src
VPATH += ./lite-thread/common
VPATH += ./lite-thread/common/console
VPATH += ./lite-thread/common/container
VPATH += ./lite-thread/common/log

# C source files
SOURCES += ./lite-thread/src/lt_task.c
SOURCES += ./lite-thread/src/lt_message.c
SOURCES += ./lite-thread/src/lt_timer.c
SOURCES += ./lite-thread/src/lt_fsm.c

SOURCES += ./lite-thread/common/lt_common.c
SOURCES += ./lite-thread/common/cmd_line.c
SOURCES += ./lite-thread/common/utils.c
SOURCES += ./lite-thread/common/console/xprintf.c
SOURCES += ./lite-thread/common/console/lt_console.c
SOURCES += ./lite-thread/common/console/task_console.c
SOURCES += ./lite-thread/common/container/lt_shell.c
SOURCES += ./lite-thread/common/container/ring_buffer.c
SOURCES += ./lite-thread/common/log/lt_log.c
