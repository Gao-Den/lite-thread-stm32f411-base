##############################################################################
 # @author: GaoDen
 # @date:   03/05/2024
##############################################################################

# Include sources file
include lite-thread/Makefile.mk
include app/Makefile.mk
include common/Makefile.mk
include drivers/Makefile.mk
include sys/Makefile.mk
include libraries/Makefile.mk
include networks/Makefile.mk

# Utilitis define
Print = @echo "~"
print = @echo

# Name of build PROJECT ex: lite-thread-stm32f411-base.bin
NAME_MODULE = lite-thread-stm32f411-base
PROJECT = $(NAME_MODULE)
OBJECTS_DIR = build
TARGET = $(OBJECTS_DIR)/$(NAME_MODULE).axf

OBJECTS = $(addprefix $(OBJECTS_DIR)/,$(notdir $(SOURCES_ASM:.s=.o)))
OBJECTS += $(addprefix $(OBJECTS_DIR)/,$(notdir $(SOURCES:.c=.o)))
OBJECTS += $(addprefix $(OBJECTS_DIR)/,$(notdir $(SOURCES_CPP:.cpp=.o)))

GCC_PATH		= $(HOME)/workspace/tools/gcc-arm-none-eabi-10.3-2021.10
PROGRAMER_PATH		= $(HOME)/workspace/tools/STM32CubeProgrammer/bin

# App start address, that need sync with declare in linker file and interrupt vector table.
APP_START_ADDR_VAL = 0x08000000

OPTIMIZE_OPTION = -g -Os

LIBC		= $(GCC_PATH)/arm-none-eabi/lib/thumb/v7e-m+fp/hard/libc.a
LIBM		= $(GCC_PATH)/arm-none-eabi/lib/thumb/v7e-m+fp/hard/libm.a
LIBFPU		= $(GCC_PATH)/arm-none-eabi/lib/thumb/v7e-m+fp/hard/libg.a
LIBRDPMON	= $(GCC_PATH)/arm-none-eabi/lib/thumb/v7e-m+fp/hard/librdpmon.a
LIBSTDCPP_NANO	= $(GCC_PATH)/arm-none-eabi/lib/thumb/v7e-m+fp/hard/libstdc++_nano.a

LIBGCC		= $(GCC_PATH)/lib/gcc/arm-none-eabi/10.3.1/thumb/v7e-m+fp/hard/libgcc.a
LIBGCOV		= $(GCC_PATH)/lib/gcc/arm-none-eabi/10.3.1/thumb/v7e-m+fp/hard/libgcov.a

LIB_PATH += -L$(GCC_PATH)/arm-none-eabi/lib/thumb/v7e-m+fp/hard
LIB_PATH += -L$(GCC_PATH)/lib/gcc/arm-none-eabi/10.3.1/thumb/v7e-m+fp/hard

# The command for calling the compiler.
CC		=	$(GCC_PATH)/bin/arm-none-eabi-gcc
CPP		=	$(GCC_PATH)/bin/arm-none-eabi-g++
AR		=	$(GCC_PATH)/bin/arm-none-eabi-ar
AS		=	$(GCC_PATH)/bin/arm-none-eabi-gcc -x assembler-with-cpp
LD 		= 	$(GCC_PATH)/bin/arm-none-eabi-ld
OBJCOPY		=	$(GCC_PATH)/bin/arm-none-eabi-objcopy
OBJNM		=	$(GCC_PATH)/bin/arm-none-eabi-nm
ARM_SIZE	=	$(GCC_PATH)/bin/arm-none-eabi-size

# Set the compiler CPU/FPU options.
CPU = -mthumb -mcpu=cortex-m4
FPU = -mfloat-abi=hard

GENERAL_FLAGS +=			\
		$(OPTIMIZE_OPTION)	\
		-DNDEBUG	\
		-DUSE_STDPERIPH_DRIVER	\
		-DSTM32F411xE	\

# C compiler flags
CFLAGS +=	\
		$(CPU)			\
		$(FPU)			\
		-ffunction-sections	\
		-fdata-sections		\
		-fstack-usage		\
		-MD			\
		-Wall			\
		-Wno-enum-conversion	\
		-Wno-redundant-decls	\
		-std=c99		\
		-c			\
		$(GENERAL_FLAGS)	\

# C++ compiler flags
CPPFLAGS += $(CPU)			\
		$(FPU)			\
		-ffunction-sections	\
		-fdata-sections		\
		-fstack-usage		\
		-fno-rtti		\
		-fno-exceptions		\
		-fno-use-cxa-atexit	\
		-MD			\
		-Wall			\
		-std=c++11		\
		-c			\
		$(GENERAL_FLAGS)	\

# linker file
LDFILE = sys/startup/stm32f411ce.ld

# linker flags
LDFLAGS	=	-Map=$(OBJECTS_DIR)/$(PROJECT).map	\
		--gc-sections	\
		$(LIB_PATH)	\
		$(LIBC) $(LIBM) $(LIBSTDCPP_NANO) $(LIBGCC) $(LIBGCOV) $(LIBFPU) $(LIBRDPMON)

all: build $(TARGET)

build:
	$(Print) CREATE $(OBJECTS_DIR) folder
	@mkdir -p $(OBJECTS_DIR)

$(TARGET): $(OBJECTS) $(LIBC) $(LIBM) $(LIBSTDCPP_NANO) $(LIBGCC) $(LIBGCOV) $(LIBFPU) $(LIBRDPMON)
	$(Print) LD $@
	@$(LD) --entry reset_handler -T $(LDFILE) $(LDFLAGS) -o $(@) $(^)
	$(Print) OBJCOPY $(@:.axf=.bin)
	@$(OBJCOPY) -O binary $(@) $(@:.axf=.bin)
	@$(OBJCOPY) -O binary $(@) $(@:.axf=.out)
	@$(OBJCOPY) -O binary $(@) $(@:.axf=.elf)
	@$(ARM_SIZE) $(TARGET)

$(OBJECTS_DIR)/%.o: %.c
	$(Print) CC $@
	@$(CC) $(CFLAGS) -o $@ $<

$(OBJECTS_DIR)/%.o: %.cpp
	$(Print) CXX $@
	@$(CPP) $(CPPFLAGS) -o $@ $<

$(OBJECTS_DIR)/%.o: %.s
	$(Print) AS $@
	@$(AS) $(CFLAGS) -o $@ $<

# # For Linux
# flash: all
# 	$(PROGRAMER_PATH)/STM32_Programmer.sh -c port=SWD -w $(TARGET:.axf=.bin) $(APP_START_ADDR_VAL) -rst

# clean:
# 	$(Print) CLEAN $(OBJECTS_DIR) folder
# 	@rm -rf $(OBJECTS_DIR)

# For Windows
clean:
	$(Print) CLEAN $(OBJECTS_DIR) folder
	@if exist build (rmdir /s /q $(OBJECTS_DIR) -p)

flash: all
	st-link_cli.exe -c SWD -P $(TARGET:.axf=.bin) $(APP_START_ADDR_VAL) -rst

sym: $(TARGET)
	$(Print) export object name $(<:.axf=.sym)
	$(OBJNM) --size-sort --print-size $(<) >> $(<:.axf=.sym)

view_sym:
	cat $(OBJECTS_DIR)/$(NAME_MODULE).sym

help:
	$(print) "How to use?"
	$(print) ""

	$(print) "[make build] complile the code"
	$(print) "[make flash] burn firmware via st-link"
	$(print) "[make clean] clean build project folder"
	$(print) "[make sym] create list symbol fromx objects file"
	$(print) "[make view_sym] view list symbol size"
	$(print) ""

# architecture options usage
#--------------------------------------------------------------------
#| ARM Core | Command Line Options                       | multilib |
#|----------|--------------------------------------------|----------|
#|Cortex-M0+| -mthumb -mcpu=cortex-m0plus                | armv6-m  |
#|Cortex-M0 | -mthumb -mcpu=cortex-m0                    |          |
#|Cortex-M1 | -mthumb -mcpu=cortex-m1                    |          |
#|          |--------------------------------------------|          |
#|          | -mthumb -march=armv6-m                     |          |
#|----------|--------------------------------------------|----------|
#|Cortex-M3 | -mthumb -mcpu=cortex-m3                    | armv7e-m  |
#|          |--------------------------------------------|          |
#|          | -mthumb -march=armv7e-m                     |          |
#|----------|--------------------------------------------|----------|
#|Cortex-M4 | -mthumb -mcpu=cortex-m4                    | armv7e-m |
#|(No FP)   |--------------------------------------------|          |
#|          | -mthumb -march=armv7e-m                    |          |
#|----------|--------------------------------------------|----------|
#|Cortex-M4 | -mthumb -mcpu=cortex-m4 -mfloat-abi=softfp | armv7e-m |
#|(Soft FP) | -mfpu=fpv4-sp-d16                          | /softfp  |
#|          |--------------------------------------------|          |
#|          | -mthumb -march=armv7e-m -mfloat-abi=softfp |          |
#|          | -mfpu=fpv4-sp-d16                          |          |
#|----------|--------------------------------------------|----------|
#|Cortex-M4 | -mthumb -mcpu=cortex-m4 -mfloat-abi=hard   | armv7e-m |
#|(Hard FP) | -mfpu=fpv4-sp-d16                          | /fpu     |
#|          |--------------------------------------------|          |
#|          | -mthumb -march=armv7e-m -mfloat-abi=hard   |          |
#|          | -mfpu=fpv4-sp-d16                          |          |
#|----------|--------------------------------------------|----------|
#|Cortex-R4 | [-mthumb] -march=armv7-r                   | armv7-ar |
#|Cortex-R5 |                                            | /thumb   |
#|Cortex-R7 |                                            |      |
#|(No FP)   |                                            |          |
#|----------|--------------------------------------------|----------|
#|Cortex-R4 | [-mthumb] -march=armv7-r -mfloat-abi=softfp| armv7-ar |
#|Cortex-R5 | -mfpu=vfpv3-d16                            | /thumb   |
#|Cortex-R7 |                                            | /softfp  |
#|(Soft FP) |                                            |          |
#|----------|--------------------------------------------|----------|
#|Cortex-R4 | [-mthumb] -march=armv7-r -mfloat-abi=hard  | armv7-ar |
#|Cortex-R5 | -mfpu=vfpv3-d16                            | /thumb   |
#|Cortex-R7 |                                            | /fpu     |
#|(Hard FP) |                                            |          |
#|----------|--------------------------------------------|----------|
#|Cortex-A* | [-mthumb] -march=armv7-a                   | armv7-ar |
#|(No FP)   |                                            | /thumb   |
#|----------|--------------------------------------------|----------|
#|Cortex-A* | [-mthumb] -march=armv7-a -mfloat-abi=softfp| armv7-ar |
#|(Soft FP) | -mfpu=vfpv3-d16                            | /thumb   |
#|          |                                            | /softfp  |
#|----------|--------------------------------------------|----------|
#|Cortex-A* | [-mthumb] -march=armv7-a -mfloat-abi=hard  | armv7-ar |
#|(Hard FP) | -mfpu=vfpv3-d16                            | /thumb   |
#|          |                                            | /fpu     |
#--------------------------------------------------------------------
