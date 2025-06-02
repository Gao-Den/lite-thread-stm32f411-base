
CFLAGS += -I./sys/peripheral/peripheral_libraries/STM32F4xx_StdPeriph_Driver/inc
CPPFLAGS += -I./sys/peripheral/peripheral_libraries/STM32F4xx_StdPeriph_Driver/inc

VPATH += ./sys/peripheral/peripheral_libraries/STM32F4xx_StdPeriph_Driver/src

# C source files
SOURCES += ./sys/peripheral/peripheral_libraries/STM32F4xx_StdPeriph_Driver/src/misc.c
SOURCES += ./sys/peripheral/peripheral_libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_gpio.c
SOURCES += ./sys/peripheral/peripheral_libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rcc.c
SOURCES += ./sys/peripheral/peripheral_libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_usart.c
SOURCES += ./sys/peripheral/peripheral_libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_i2c.c
SOURCES += ./sys/peripheral/peripheral_libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_spi.c
SOURCES += ./sys/peripheral/peripheral_libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_flash.c
SOURCES += ./sys/peripheral/peripheral_libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dma.c
SOURCES += ./sys/peripheral/peripheral_libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_iwdg.c
SOURCES += ./sys/peripheral/peripheral_libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_tim.c
SOURCES += ./sys/peripheral/peripheral_libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_exti.c
SOURCES += ./sys/peripheral/peripheral_libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_syscfg.c
