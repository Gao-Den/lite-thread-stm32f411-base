CFLAGS	+= -I./networks/nrf24/hal
CPPFLAGS	+= -I./networks/nrf24/hal

VPATH += ./networks/nrf24/hal

#C source files
SOURCES += ./networks/nrf24/hal/hal_nrf_hw.c
SOURCES += ./networks/nrf24/hal/hal_nrf_l01.c
