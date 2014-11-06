
LD_LIBRARY_PATH=/modules/cs132/arm-toolchain/lib
export LD_LIBRARY_PATH

OBJS_DIR = /courses/cs132/arm-stuff
ARM_TOOLS = /modules/cs132/arm-toolchain/bin
CC = $(ARM_TOOLS)/arm-none-eabi-gcc
OBJ_COPY = $(ARM_TOOLS)/arm-none-eabi-objcopy

CFLAGS  = -g -mcpu=arm7tdmi -std=c99
LDFLAGS = -T$(OBJS_DIR)/linker.cmd -nostartfiles
OBJS    = crt.o isrsupport.o lowlevelinit.o main.o
EXE     = example2.elf


% : %.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@.elf \
	$(OBJS_DIR)/crt.o $(OBJS_DIR)/isrsupport.o \
	$(OBJS_DIR)/lowlevelinit.o $(OBJS_DIR)/interrupts.o $?

	$(OBJ_COPY) -O ihex $@.elf $@.hex
