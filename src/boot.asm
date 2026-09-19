bits 32                         ; El bootloader Multiboot entra directamente en 32 bits
section .multiboot
    align 4
    dd 0x1BADB002               ; Número mágico para que QEMU reconozca el kernel
    dd 0x00                     ; Flags (0 porque no pedimos módulos extra)
    dd - (0x1BADB002 + 0x00)    ; Checksum (La suma de los tres debe dar 0)

section .text
global _start
extern kernel_main              ; Declaramos la función que está en tu kernel.c

_start:
    ; --- PREPARACIÓN DEL ENTORNO ---
    cli                         ; Desactivamos interrupciones para evitar reinicios
    mov esp, stack_space        ; Configuramos el puntero de pila (stack)
    
    ; --- SALTO AL KERNEL ---
    call kernel_main            ; Llamamos a tu función en C

    ; --- BUCLE DE SEGURIDAD ---
hang:
    hlt                         ; Detiene el procesador si el kernel termina
    jmp hang

section .bss
resb 8192                       ; Reservamos 8KB para la pila (stack)
stack_space: