ISO_DIR = iso
KERNEL = pavilionix86.bin
ISO = 1024OS.iso

LD_EMULATION ?= $(shell ld --help 2>&1 | grep -q elf_i386 && echo elf_i386 || echo i386pe)

all: $(ISO)
	@echo "-> iso generated successfully!"

src/boot.o: src/boot.asm
	nasm -f elf32 src/boot.asm -o src/boot.o

src/kernel.o: src/kernel.c
	gcc -m32 -ffreestanding -fno-pic -fno-stack-protector -Isrc -c src/kernel.c -o src/kernel.o

$(KERNEL): src/boot.o src/kernel.o
	ld -m $(LD_EMULATION) -T src/linker.ld src/boot.o src/kernel.o -o $(KERNEL)
	mkdir -p $(ISO_DIR)
	mv $(KERNEL) $(ISO_DIR)/
	rm -f src/*.o

$(ISO): $(KERNEL)
	rm -f $(ISO)
	xorriso -as mkisofs -o $(ISO) -b isolinux.bin -c boot.cat -no-emul-boot -boot-load-size 4 -boot-info-table $(ISO_DIR)

clean:
	rm -f src/*.o $(ISO) $(ISO_DIR)/$(KERNEL)
