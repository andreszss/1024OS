ENTRY(_start)                /* El punto de entrada es _start en boot.asm */
SECTIONS {
    . = 1M;                  /* Cargamos el kernel en el primer Megabyte */
    .text : { 
        *(.multiboot)        /* La cabecera SIEMPRE va primero */
        *(.text) 
    }
    .data : { *(.data) }
    .bss  : { *(.bss) }
}