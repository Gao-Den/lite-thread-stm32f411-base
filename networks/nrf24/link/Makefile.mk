CFLAGS	+= -I./networks/nrf24/link
CPPFLAGS	+= -I./networks/nrf24/link

VPATH += ./networks/nrf24/link

#C source files
SOURCES += ./networks/nrf24/link/nrf_data.c
SOURCES += ./networks/nrf24/link/nrf_phy.c
SOURCES += ./networks/nrf24/link/nrf_mac.c
SOURCES += ./networks/nrf24/link/nrf_link.c
