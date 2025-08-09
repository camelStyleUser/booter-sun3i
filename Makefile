CROSSCOMPILE = arm-none-eabi-
CROSS_CC = ${CROSSCOMPILE}gcc
ARM_ELF_FLAGS = -Os -marm -fpic -Wall
ARM_ELF_FLAGS += -fno-common -fno-builtin -ffreestanding -nostdinc -fno-strict-aliasing
ARM_ELF_FLAGS += -mno-thumb-interwork -fno-stack-protector -fno-toplevel-reorder
ARM_ELF_FLAGS += -Wstrict-prototypes -Wno-format-nonliteral -Wno-format-security
BOOTER_OBJECTS = head.o stage2.o main.o slib.o mmclib.o mmcdrv.o
MKSUNXIBOOT = ./mksunxiboot
all: booter.sunxi
	@echo "to load to sdcard do 'dd if=booter.sunxi of=/dev/sdX bs=1024 seek=8'"
%.o: %.c
	$(CROSS_CC) -c -march=armv5te -static-libgcc -nostdlib $(ARM_ELF_FLAGS) $< -nostdlib -o $@
%.o: %.S
	$(CROSS_CC) -c -march=armv5te -static-libgcc -nostdlib $(ARM_ELF_FLAGS) $< -nostdlib -o $@
booter.elf: booter.lds $(BOOTER_OBJECTS)
	$(CROSS_CC) -march=armv5te -static-libgcc -nostdlib $(ARM_ELF_FLAGS) -o $@ -T $^ -lgcc #hack, booter.lds is passed first so it works
mksunxiboot: mksunxiboot.c
	$(CC) mksunxiboot.c -o mksunxiboot
%.bin: %.elf
	$(CROSSCOMPILE)objcopy $< -O binary $@
.NOTINTERMEDIATE: booter.bin #make, booter.bin is not an intermediate file, please do not remove it
%.sunxi: %.bin mksunxiboot
	$(MKSUNXIBOOT) $< $@
clean:
	$(RM) -f *.elf *.bin mksunxiboot *.sunxi *.o
.PHONY: all clean
