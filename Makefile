ISO_DIR = iso
KERNEL = pavilionix86.bin
ISO = 1024OS.iso

ifeq ($(OS),Windows_NT)
define LINK_KERNEL
	ld -m i386pe -T src/linker.ld src/boot.o src/kernel.o -o temp.elf
	objcopy -O elf32-i386 temp.elf $(KERNEL)
	rm -f temp.elf
endef
else
define LINK_KERNEL
	ld -m elf_i386 -T src/linker.ld src/boot.o src/kernel.o -o $(KERNEL)
endef
endif

all: $(ISO)
	@echo "-> iso generada correctamente (michi gay)"

src/boot.o: src/boot.asm
	nasm -f elf32 src/boot.asm -o src/boot.o

src/kernel.o: src/kernel.c
	gcc -m32 -ffreestanding -fno-pic -fno-stack-protector -Isrc -c src/kernel.c -o src/kernel.o

$(KERNEL): src/boot.o src/kernel.o
	$(call LINK_KERNEL)
	mkdir -p $(ISO_DIR)
	mv $(KERNEL) $(ISO_DIR)/
	rm -f src/*.o

$(ISO): $(KERNEL)
	rm -f $(ISO)
	xorriso -as mkisofs -o $(ISO) -b isolinux.bin -c boot.cat -no-emul-boot -boot-load-size 4 -boot-info-table $(ISO_DIR)

clean:
	rm -f src/*.o temp.elf $(ISO) $(ISO_DIR)/$(KERNEL)
