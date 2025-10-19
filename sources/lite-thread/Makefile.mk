INCLUDE_FLAGS += -I./sources/lite-thread/inc
INCLUDE_FLAGS += -I./sources/lite-thread/common
INCLUDE_FLAGS += -I./sources/lite-thread/common/console
INCLUDE_FLAGS += -I./sources/lite-thread/common/container
INCLUDE_FLAGS += -I./sources/lite-thread/common/log

VPATH += ./sources/lite-thread/src
VPATH += ./sources/lite-thread/common
VPATH += ./sources/lite-thread/common/console
VPATH += ./sources/lite-thread/common/container
VPATH += ./sources/lite-thread/common/log

# C source files
SOURCE_C += lt_task.c
SOURCE_C += lt_message.c
SOURCE_C += lt_timer.c
SOURCE_C += lt_fsm.c

SOURCE_C += lt_common.c
SOURCE_C += cmd_line.c
SOURCE_C += utils.c
SOURCE_C += xprintf.c
SOURCE_C += lt_console.c
SOURCE_C += task_console.c
SOURCE_C += lt_shell.c
SOURCE_C += ring_buffer.c
SOURCE_C += lt_log.c
