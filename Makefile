EXECUTABLE=firmware.elf
BIN_IMAGE=firmware.bin

CC=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy

CFLAGS=-g -Os -mlittle-endian -mthumb
CFLAGS+=-mcpu=cortex-m0	
CFLAGS+=-ffreestanding -nostdlib

# to run from FLASH
CFLAGS+=-Wl,-T,linker.ld

# for the libopencm3 lib
CFLAGS+=-I$(OPENCM3_DIR)/include -DSTM32F0 -fno-common -msoft-float

all: $(BIN_IMAGE)

$(BIN_IMAGE): $(EXECUTABLE)
	$(OBJCOPY) -O binary $^ $@

$(EXECUTABLE): main.c linker.ld 
	$(CC) $(CFLAGS) $^ -o $@  -L$(OPENCM3_DIR)/lib/stm32/f0 -lopencm3_stm32f0
	
install:
	st-flash write $(BIN_IMAGE) 0x08000000

clean:
	rm -rf $(EXECUTABLE)
	rm -rf $(BIN_IMAGE)

.PHONY: all clean
