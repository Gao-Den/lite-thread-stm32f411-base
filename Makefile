###############################################################################
# author: GaoDen
# date: 10/09/2025
###############################################################################

NAME_MODULE	= lite-thread-stm32f411-base
PROJECT = $(NAME_MODULE)
OBJ_DIR = build
TARGET = $(OBJ_DIR)/$(NAME_MODULE).axf

include sources/lite-thread/Makefile.mk
include sources/app/Makefile.mk
include sources/common/Makefile.mk
include sources/drivers/Makefile.mk
include sources/sys/Makefile.mk
include sources/libraries/Makefile.mk
include sources/networks/Makefile.mk

# the compiler to use
GCC_PATH = /mnt/d/GaoDen/dev/tools/gcc-arm-none-eabi-10.3-2021.10
PROGRAMER_PATH = /mnt/d/GaoDen/dev/tools/STM32CubeProgrammer/bin

CC = $(GCC_PATH)/bin/arm-none-eabi-gcc
CXX = $(GCC_PATH)/bin/arm-none-eabi-g++
AR = $(GCC_PATH)/bin/arm-none-eabi-ar
AS = $(GCC_PATH)/bin/arm-none-eabi-gcc -x assembler-with-cpp
LD = $(GCC_PATH)/bin/arm-none-eabi-ld
OBJCOPY = $(GCC_PATH)/bin/arm-none-eabi-objcopy
OBJNM = $(GCC_PATH)/bin/arm-none-eabi-nm
ARM_SIZE = $(GCC_PATH)/bin/arm-none-eabi-size

# build options
OPTIMIZE_OPTION += -Os -g
WARNING_OPTION	+= -Werror -Wno-missing-field-initializers

# library source path (sources ".so, .a")
LIBC = $(GCC_PATH)/arm-none-eabi/lib/thumb/v7e-m+fp/hard/libc.a
LIBM = $(GCC_PATH)/arm-none-eabi/lib/thumb/v7e-m+fp/hard/libm.a
LIBFPU = $(GCC_PATH)/arm-none-eabi/lib/thumb/v7e-m+fp/hard/libg.a
LIBRDPMON = $(GCC_PATH)/arm-none-eabi/lib/thumb/v7e-m+fp/hard/librdpmon.a
LIBSTDCPP_NANO = $(GCC_PATH)/arm-none-eabi/lib/thumb/v7e-m+fp/hard/libstdc++_nano.a

LIBGCC = $(GCC_PATH)/lib/gcc/arm-none-eabi/10.3.1/thumb/v7e-m+fp/hard/libgcc.a
LIBGCOV = $(GCC_PATH)/lib/gcc/arm-none-eabi/10.3.1/thumb/v7e-m+fp/hard/libgcov.a

LIB_PATH += -L$(GCC_PATH)/arm-none-eabi/lib/thumb/v7e-m+fp/hard
LIB_PATH += -L$(GCC_PATH)/lib/gcc/arm-none-eabi/10.3.1/thumb/v7e-m+fp/hard

LDLIBS += $(LIBC) $(LIBM) $(LIBSTDCPP_NANO) $(LIBGCC) $(LIBGCOV) $(LIBFPU) $(LIBRDPMON)

LDFLAGS	= -Map=$(OBJ_DIR)/$(PROJECT).map	\
			--gc-sections	\
			$(LIB_PATH)	\

# object files
OBJECTS += $(patsubst %.s, $(OBJ_DIR)/%.o, $(SOURCE_AS))
OBJECTS = $(patsubst %.c, $(OBJ_DIR)/%.o, $(SOURCE_C))
OBJECTS += $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(SOURCE_CPP))

# linker file
LDFILE = sources/sys/startup/stm32f411ce.ld

# define options
APP_START_ADDR = 0x08000000
APP_MODE += -DAPP_DEBUG
DEFINE_OPTIONS += $(APP_MODE)	\
				-DUSE_STDPERIPH_DRIVER	\
				-DSTM32F411xE	\
				
# build flags
GENERAL_FLAG += $(OPTIMIZE_OPTION)	\
				$(WARNING_OPTION) \
				$(INCLUDE_FLAGS)	\
				$(DEFINE_OPTIONS)	\
				-mthumb -mcpu=cortex-m4	\
				-mfloat-abi=hard	\
				-ffunction-sections	\
				-fdata-sections	\
				-fstack-usage	\
				-MD	\
				-Wall	\
				-c	\
				-fno-common

CFLAGS += $(GENERAL_FLAG)	\
			-std=c99	\
			-Wno-enum-conversion	\
			-Wno-redundant-decls	\

CXXFLAGS += $(GENERAL_FLAG)	\
			-std=c++11	\
			-fno-rtti	\
			-fno-exceptions	\
			-fno-use-cxa-atexit	\

.PHONY: all
all: create $(TARGET)

create:
	@echo "[Compiling sources]"
	@mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: %.s
	@echo "~ AS $<"
	@$(AS) $< -o $@

$(OBJ_DIR)/%.o: %.c
	@echo "~ CC $<"
	@$(CC) -c $< -o $@ $(CFLAGS)

$(OBJ_DIR)/%.o: %.cpp
	@echo "~ CPP $<"
	@$(CXX) -c $< -o $@ $(CXXFLAGS)

$(TARGET): $(OBJECTS) $(LDLIBS)
	@echo "\n[Linking objects]"
	@$(foreach obj,$(OBJECTS),echo "$(notdir $(obj))";)
	@$(LD) --entry reset_handler -T $(LDFILE) $(LDFLAGS) -o $(@) $(^)

	@echo OBJCOPY $(@:.axf=.bin)
	@$(OBJCOPY) -O binary $(@) $(@:.axf=.bin)
	@$(OBJCOPY) -O ihex   $(@) $(@:.axf=.hex)
	@cp $(@) $(@:.axf=.elf)
	@$(ARM_SIZE) $(TARGET)

.PHONY: flash
flash:
	@echo "flashing firmware $(NAME_MODULE).bin to target"
	@sudo st-flash write $(TARGET:.axf=.bin) $(APP_START_ADDR)
	@sudo st-flash reset

.PHONY: clean
clean:
	rm -rf $(OBJ_DIR)

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

# optimization options usage
#-------------------------------------------------------------------- 
#| Option | Purpose / Use Case                            | Typical Use |
#|---------|-----------------------------------------------|--------------|
#| -O0     | no optimization (full debug info, fast build) | debugging / development |
#|---------|-----------------------------------------------|--------------|
#| -O1     | basic optimization without long compile time  | safe optimization |
#|---------|-----------------------------------------------|--------------|
#| -O2     | further optimization (balance size/speed)     | general release build |
#|---------|-----------------------------------------------|--------------|
#| -O3     | aggressive optimization (loop unrolling, etc) | performance-critical code |
#|---------|-----------------------------------------------|--------------|
#| -Ofast  | enable all -O3 + unsafe math optimizations     | high-speed, non-strict IEEE math |
#|---------|-----------------------------------------------|--------------|
#| -Os     | optimize for size (reduces code size)         | flash-limited MCUs |
#|---------|-----------------------------------------------|--------------|
#| -Og     | optimize for debugging (keep symbols, partial)| debug build with light opt |
#|---------|-----------------------------------------------|--------------|
#| -Oz     | (clang only) smaller than -Os                | N/A for GCC |
#|---------|-----------------------------------------------|--------------|
#| -flto   | link-time optimization across all units       | advanced release optimization |
#|---------|-----------------------------------------------|--------------|
#| -g      | include debug symbols                         | debug/release with trace |
#|---------|-----------------------------------------------|--------------|
