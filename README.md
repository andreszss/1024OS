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
- **Linux:**
 - nasm
 - gcc
 - binutils
 - xorriso
 - make
- **Windows:**
 - mingw-w64
 - nasm
 - xorriso
 - make

## Como obtener las dependencias
- **Linux:**
 - **Base Debian (Ubuntu, Linux Mint, etc):**
  ```shell
  # apt update
  # apt install build-essential nasm xorriso
  ```

  - **Base RHEL (Fedora, Rocky Linux, etc):**
  ```shell
  # dnf install @development-tools nasm xorriso
  ```

  - **Base Arch (Manjaro, CachyOS, etc):**
  ```shell
  # pacman -S base-devel nasm xorriso
  ```

  - **Gentoo:**
  ```shell
  # emerge --ask dev-lang/nasm dev-util/make sys-devel/gcc sys-devel/binutils dev-libs/xorriso
  ```

  - **Alpine: **
  ```shell
  # apk add build-base nasm xorriso
  ```

- **Windows:**
 1) Descarga [MSYS2](https://msys2.org)

 2) Abre la terminal de MSYS2

 3) Actualiza los paquetes:
 ```shell
 $ pacman -Syu
 ```

 4) Instala las dependencias:
 ```shell
 $ pacman -S mingw-w64-toolchain nasm make xorriso
 ```

## Compilar
 1) Clonar el repositorio:
 ```shell
 $ git clone https://github.com/andrszss/1024OS.git
 $ cd 1024OS
 ```
 2) Compilar:
 ```shell
 $ make
 ```

## Compilar (manualmente)
 1) Clonar el repositorio:
```shell
$ git clone https://github.com/andreszss/1024OS.git
$ cd 1024OS
```
 2) Compilar el boot.asm:
```shell
$ nasm -f elf32 src/boot.asm -o src/boot.o
```
 3) Compilar el binario:
```shell
$ gcc -m32 -ffreestanding -fno-pic -fno-stack-protector -Isrc -c src/kernel.c -o src/kernel.o
```
 4) Enlazar el kernel:
```shell
$ ld -m elf_i386 -T src/linker.ld src/boot.o src/kernel.o -o pavilionix86.bin
```
 5) Mover el kernel compilado a la carpeta iso:
```shell
$ mv pavilionix86.bin iso
```
 6) Generar la ISO:
```shell
$ xorriso -as mkisofs -o 1024OS.iso -b isolinux.bin -c boot.cat -no-emul-boot -boot-load-size 4 -boot-info-table iso
```
 7) Limpiar archivos temporales:
```shell
$ rm src/*.o
```

---

