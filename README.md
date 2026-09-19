# 1024OS
~ Sistema simple hecho en C y Assembly. ~

## Que es 1024OS?
1024OS es un sistema operativo ligero, hecho en C y Assembly, usando el kernel Pavilionix86.
Este proyecto es un experimento de desarrollo de sistemas operativos desde cero, optimizado para ejecutarse en hardware real y emuladores.

## Caracteristicas
- **No necesita instalacion** → Es un LiveCD que de momento, no necesita instalacion.
- **Compatible en PCs viejas** → Probado en una pc con 2mb de RAM.
- **Multiboot** → Compatible con cargadores de arranque actuales (GRUB/Syslinux)
- **Hecho en C** → Su codigo principal, esta hecho en C, lo que le da velocidad al kernel.
- **Ligero** → El kernel actual, pesa 10,9kb
- **Simple** → Una terminal simple, tenemos planeado hacer una GUI.  

## Requisitos
- **CPU:** Intel 80386 (i386)
- **RAM:** 2MB (o hasta menos)
- Al menos un **CD de 1MB** (no hemos probado usar el sistema con un pendrive, ni disquete.)
- **BIOS:** Legacy (UEFI no funciona)
- **Monitor:** Compatible con VGA

## Dependencias
 - nasm
 - gcc
 - binutils
 - xorriso / mtools
 (Busca como instalarlos en tu distribucion, yo uso Arch y me va bien.)

## Compilar
 1) Clonar el repositorio:
```bash
git clone https://github.com/AndresBDW/1024OS.git
cd 1024OS
```
 2) Compilar el boot.asm:
```bash
nasm -f elf32 boot.asm -o boot.o
```
 3) Compilar el binario:
```bash
gcc -m32 -ffreestanding -fno-pic -fno-stack-protector -c kernel.c -o kernel.o
```
 4) Enlazar el kernel:
```bash
ld -m elf_i386 -T linker.ld boot.o kernel.o -o pavilionix86.bin
```
 5) Mover el kernel compilado a la carpeta ISO:
```bash
mv pavilionix86.bin ISO
```
 6) Generar la ISO:
```bash
xorriso -as mkisofs -o 1024OS.iso -b isolinux.bin -c boot.cat -no-emul-boot -boot-load-size 4 -boot-info-table ISO
```
 7) Limpiar archivos temporales:
```bash
rm *.o
```

**o, simplemente, usar el script "build.sh".**

---

