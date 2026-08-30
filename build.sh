#!/bin/bash -e

ISO_DIR="ISO"
KERNEL_NAME="pavilionix86.bin"

log() {
    printf '\033[32m->\033[m %s\n' "$*"
}


err() {
    printf '\033[31m->\033[m %s\n' "$*" >&2
    exit 1
}

checkdependencies() {
	if command -v nasm &> /dev/null; then
		log found nasm...
	else
		err nasm not found!
	fi
	
	if command -v gcc &> /dev/null; then
		log found gcc...
	else
		err gcc not found!
	fi

	if command -v ld &> /dev/null; then
		log found ld...
	else 
		err ld not found!
	fi

	if command -v xorriso &> /dev/null; then
		log found xorriso...
	else
		err xorriso not found!
	fi
}

main() {
	log 1024OS iso builder

	log 1. checking dependencies
	checkdependencies

	log 2. building
	nasm -f elf32 boot.asm -o boot.o 
	gcc -m32 -ffreestanding -fno-pic -fno-stack-protector -c kernel.c -o kernel.o
	log 3. linking
	ld -m elf_i386 -T linker.ld boot.o kernel.o -o $KERNEL_NAME 

	log 4. updating $ISO_DIR
	mv $KERNEL_NAME $ISO_DIR/ 

	log 5. building iso
	rm -f 1024OS.iso
	xorriso -as mkisofs -o 1024OS.iso -b isolinux.bin -c boot.cat -no-emul-boot -boot-load-size 4 -boot-info-table $ISO_DIR

	log 6. cleaning
	rm *.o 

	log iso generated successfully!
}

main
