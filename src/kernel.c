// -------------------------------------------------
// 1024OS - Un OS Simple
// - Por Andresqwq
// -------------------------------------------------

// -------------------------------------------------
// /* --- COMUNICACION CON EL CPU (I/O PORTS) --- */    
// -------------------------------------------------

#include <sys/cpu.h>

// ------------------------------------------
// /* --- RECURSOS DEL SISTEMA Y VIDEO --- */
// ------------------------------------------

#include <sys/sysrecurses.h>

// --------------------------------------------
// /* --- UTILIDADES DE PANTALLA Y DELAY --- */
// --------------------------------------------

#include <sys/screen.h>

// -----------------------------------------
// /* --- FUNCIONES DE NUCLEO Y TEXTO --- */
// -----------------------------------------

#include <sys/functions.h>

// ------------------------------------
// /* --- PROCESADOR DE COMANDOS --- */
// ------------------------------------

#include <sys/commands.h>

// ---------------------
// /* --- TECLADO --- */
// ---------------------

#include <sys/keyboard.h>

// ---------------------------------------
//            KERNEL PRINCIPAL
// ---------------------------------------

void kernel_main() {
    // PRIMER BEEP
    beep(200);
    // DESACTIVAR PIC
    outb(0x21, 0xFD); // SOLO IRQ 1
    outb(0xA1, 0xFF); // DESACTIVAR PIC
    // INICIO
    clear_screen();
    print("Cargando Kernel: Pavilionix86 0.2...\n", 0x0A);
    print("Iniciando kernel: Pavilionix86 0.2...\n", 0x0A);
    print("dvInit 0.2 esta iniciando...\n", 0x0A);
    print("[OK] Limpiado de pantalla correcto.\n", 0x0A);
    print("----------------------------------\n", 0x02);
    print("Bienvenido a 1024OS!\n", 0x0A);
    print("Version: 0.4-rc2\n", 0x0F);
    print("Kernel: Pavilionix86 0.2\n", 0x0F);
    print("-> ", 0x07);

    // KERNEL
    while(1) {
        if (inb(0x64) & 0x01) {
            unsigned char sc = inb(0x60);
            if (sc == 0x2A || sc == 0x36) {
                shift_pressed = 1;
            } 
            else if (sc == 0xAA || sc == 0xB6) {
                shift_pressed = 0;
            } 
            else if (!(sc & 0x80)) {
                if (sc == 0x1C) { 
                    execute_command();
                } else if (sc == 0x0E) { 
                    if (buffer_idx > 0) {
                        buffer_idx--; 
                        term_x--;
                        video_memory[(term_y * 80 + term_x) * 2] = ' ';
                        update_cursor();
                    }
                } else {
                    char c = get_ascii(sc, shift_pressed);
                    if (c && buffer_idx < 78) {
                        command_buffer[buffer_idx++] = c;
                        char s[2] = {c, 0};
                        print(s, 0x0F);
                    }
                }
            }
        }
    }
}
