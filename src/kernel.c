#include <sys/cpu.h>
#include <sys/sysrecurses.h>
#include <sys/screen.h>
#include <sys/functions.h>
#include <sys/variables.h>
#include <sys/hardware.h>
#include <sys/commands.h>
#include <sys/keyboard.h>

void kernel_main(void) asm("kernel_main");

void kernel_main() {
    init_env_variables();
    uptime_init();
    beep(200);
    outb(0x21, 0xFD);
    outb(0xA1, 0xFF);
    clear_screen();
    
    char cpu_name[50];
    char mem_str[12];
    get_cpu_name(cpu_name);
    itoa(get_total_ram_mb(), mem_str);

    print("1024OS 0.4 (GENERIC) #0\n", 0x07);
    print("kernel: 1024 0.2\n", 0x07);
    print("cpu0: ", 0x07); print(cpu_name, 0x07); print("\n", 0x07);
    print("real mem = ", 0x07); print(mem_str, 0x07); print(" MB\n", 0x07);
    print("init: iniciando\n", 0x07);
    print("init: pantalla limpiada\n", 0x07);
    print("Bienvenido a ", 0x07); print("1024OS", 0x0A); print("!\n", 0x07);
    print("-> ", 0x07);

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
