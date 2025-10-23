CROSSCOMPILE = arm-none-eabi-
CROSS_CC = ${CROSSCOMPILE}gcc
ARM_ELF_FLAGS = -flto -Os -marm -fpic -Wall
ARM_ELF_FLAGS += -fno-common -fno-builtin -ffreestanding -nostdinc -fno-strict-aliasing
ARM_ELF_FLAGS += -mno-thumb-interwork -fno-stack-protector -fno-toplevel-reorder
ARM_ELF_FLAGS += -Wstrict-prototypes -Wno-format-nonliteral -Wno-format-security
BOOTER_OBJECTS = enter.o limpl.o main.o slib.o mmcdrv.o mmclib.o part.o mbr.o fat32.o
MKST2BOOT = ./mkst2boot
PADBT2 = ./padbt2.sh
BLKOFFSET = 48 #whatever offset you want, currently 512 sectors
all: booter2.bin booter2.desc
	@echo "to load to sdcard do 'dd if=booter2.bin of=/dev/sdX bs=512 seek=$(BLKOFFSET)' and 'dd if=booter2.desc of=/dev/sdX bs=512 seek=15'"
	@echo "you can change the offset(BLKOFFSET,currently $(BLKOFFSET)) used in the Makefile"
%.o: %.c
	$(CROSS_CC) -c -march=armv5te -static-libgcc -nostdlib $(ARM_ELF_FLAGS) $< -nostdlib -o $@
%.o: %.S
	$(CROSS_CC) -c -march=armv5te -static-libgcc -nostdlib $(ARM_ELF_FLAGS) $< -nostdlib -o $@
booter2.elf: booter2.lds $(BOOTER_OBJECTS)
	$(CROSS_CC) -march=armv5te -static-libgcc -nostdlib $(ARM_ELF_FLAGS) -o $@ -T $^ -lgcc #hack, booter.lds is passed first so it works
mkst2boot: mkst2boot.c
	$(CC) mkst2boot.c -o mkst2boot
%.bin: %.elf
	$(CROSSCOMPILE)objcopy $< -O binary $@
	$(PADBT2) "$@"
.NOTINTERMEDIATE: booter2.bin #make, booter.bin is not an intermediate file, please do not remove it
%.desc: %.bin mkst2boot
	$(MKST2BOOT) $< $(BLKOFFSET) $@
clean:
	$(RM) -f *.elf *.bin mkst2boot *.sunxi *.o *.desc
.PHONY: all clean
