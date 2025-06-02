CFLAGS += -I./sys/peripheral/peripheral_libraries/CMSIS/Include
CFLAGS += -I./sys/peripheral/peripheral_libraries/CMSIS/Device/ST/STM32F4xx/Include

CPPFLAGS += -I./sys/peripheral/peripheral_libraries/CMSIS/Include
CPPFLAGS += -I./sys/peripheral/peripheral_libraries/CMSIS/Device/ST/STM32F4xx/Include

VPATH += ./sys/peripheral/peripheral_libraries/CMSIS/Device/ST/STM32F4xx/Source/Templates

SOURCES += ./sys/peripheral/peripheral_libraries/CMSIS/Device/ST/STM32F4xx/Source/Templates/system_stm32f4xx.c
