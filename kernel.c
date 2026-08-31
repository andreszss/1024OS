// -------------------------------------------------
// 1024OS - Un OS Simple
// - Por Andresqwq
// -------------------------------------------------

// -------------------------------------------------
// /* --- COMUNICACION CON EL CPU (I/O PORTS) --- */    
// -------------------------------------------------

void outb(unsigned short port, unsigned char val) {
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

void outw(unsigned short port, unsigned short val) {
    asm volatile ( "outw %0, %1" : : "a"(val), "Nd"(port) );
}

unsigned char inb(unsigned short port) {
    unsigned char ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// ------------------------------------------
// /* --- RECURSOS DEL SISTEMA Y VIDEO --- */
// ------------------------------------------

int term_x = 0; 
int term_y = 0;
int shift_pressed = 0;
int check_keystatus() {
    return inb(0x64) & 0x01;
}

unsigned char* video_memory = (unsigned char*)0xB8000; 

// --------------------------------------------
// /* --- UTILIDADES DE PANTALLA Y DELAY --- */
// --------------------------------------------

// Actualizar cursor en pantalla
void update_cursor() {
    unsigned short pos = term_y * 80 + term_x;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (unsigned char)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (unsigned char)((pos >> 8) & 0xFF));
}

// Milisegundos reales
void sleep(int ms) {
    for (int i = 0; i < ms; i++) {
        // 1.193182 MHz
        // 1193 ciclos = 1ms
        outb(0x43, 0x00);
        unsigned char laton = inb(0x40);
        unsigned char haton = inb(0x40);
    }
}

// Scroll
void scroll() {
    if (term_y >= 25) {
        // Mover lineas hacia arriba
        for (int i = 0; i < 80 * 24 * 2; i++) {
            video_memory[i] = video_memory[i + 160];
        }

        // Limpiar la última linea (la 24)
        for (int i = 80 * 24 * 2; i < 80 * 25 * 2; i += 2) {
            video_memory[i] = ' ';
            video_memory[i + 1] = 0x07;
        }

        term_y = 24; // Mantenernos en el borde inferior
    }
}

// -----------------------------------------
// /* --- FUNCIONES DE NUCLEO Y TEXTO --- */
// -----------------------------------------

char command_buffer[80];
int buffer_idx = 0;

void beep(int duracion_ms) {
    unsigned int div = 1193180 / 750; // Frecuencia de 750 Hz
    outb(0x43, 0xB6);
    outb(0x42, (unsigned char)(div));
    outb(0x42, (unsigned char)(div >> 8));
    unsigned char tmp = inb(0x61);
    if (!(tmp & 3)) {
        outb(0x61, tmp | 3); // Enciende el pitido
    }
    for (int i = 0; i < duracion_ms * 64; i++) {
        while ((inb(0x61) & 0x10) == 0);
        while ((inb(0x61) & 0x10) != 0);
    }
    outb(0x61, inb(0x61) & 0xFC); // Apaga el pitido
}
// CLEAR_SCREEN: Limpia toda la pantalla y resetea el cursor
void clear_screen() {
    for (int i = 0; i < 80 * 25 * 2; i += 2) {
        video_memory[i] = ' '; 
        video_memory[i+1] = 0x07;
    }
    term_x = 0; term_y = 0;
    update_cursor();
}
// strcmp: Compara dos cadenas de texto
int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) { s1++; s2++; }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}
// strncmp: Compara los primeros n caracteres de dos cadenas
int strncmp(const char *s1, const char *s2, int n) {
    while (n > 0 && *s1 && (*s1 == *s2)) {
        s1++;
        s2++;
        n--;
    }
    if (n == 0) return 0;
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}
// PUT_CHAR: Imprime un caracter con un color específico
void put_char(char c, unsigned char color) {
    if (c == '\n') {
        term_x = 0;
        term_y++;
    } else {
        int index = (term_y * 80 + term_x) * 2;
        video_memory[index] = c;
        video_memory[index + 1] = color;
        term_x++;
    }
    scroll();
    update_cursor();
}
// PRINT: Imprime una cadena de texto con un color específico
void print(const char* str, unsigned char color) {
    for (int i = 0; str[i] != '\0'; i++) {
        put_char(str[i], color);
    }
}
// HALT: Detiene el sistema (usado para comandos como poweroff)
void halt() {
    while (1) {
        asm volatile ("hlt");
    }
}

// ------------------------------------
// /* --- PROCESADOR DE COMANDOS --- */
// ------------------------------------

void execute_command() {
    command_buffer[buffer_idx] = '\0';
    print("\n", 0x07);

    // COMANDOS:

    // HELP: Muestra la ayuda
    if (strcmp(command_buffer, "help") == 0) {
    print("Comandos:\n", 0x0A);
	print("help: muestra esto.\n", 0x0F);
	print("clear: limpiar la pantalla.\n", 0x0F);
	print("beep: suena un pitido.\n", 0x0F);
	print("fetch: muestra la info del sistema.\n", 0x0F);
	print("poweroff: apaga el equipo. (solo hipervisores: QEMU, VirtualBox/VMware, Bochs)\n", 0x0F);
	print("reboot: reinicia el equipo.\n", 0x0F);
	print("credits: muestra los creditos.\n", 0x0F);
	print("whoami: muestra quien eres.\n", 0x0F);
	print("echo: muestra lo que escribes.\n", 0x0F);
	print("changelog: muestra los cambios de las versiones.\n", 0x0F);
    } 

    // CLEAR: Limpiar pantalla
    else if (strcmp(command_buffer, "clear") == 0) {
        clear_screen();
        print("> ", 0x07);
    }

    // BEEP: Suena un pitido
    else if (strcmp(command_buffer, "beep") == 0) {
        print("BEEP!\n", 0x0A);
        beep(100);
    }

    // REBOOT: Reiniciar equipo
    else if (strcmp(command_buffer, "reboot") == 0) {
        outb(0x64, 0xFE);
    }

    // POWEROFF: Apagar equipo (hipervisores compatibles)
    else if (strcmp(command_buffer, "poweroff") == 0) {
        print("Apagando 1024OS...\n", 0x0C);
        outw(0x604, 0x2000);  // 1. QEMU (ACPI)
        outw(0x4004, 0x3400); // 2. VirtualBox/VMware
	    outw(0xB004, 0x2000); // 3. Bochs/QEMU viejo
        outb(0x501, 0x31);    // 4. Cloud Hypervisor / QEMU (MicroVM)
        outw(0x8900, 0x8900); // 5. Bochs Debug Port
        print("Ahora es seguro apagar con el boton\n", 0x07);
        print("Sistema en estado halt\n", 0x0C);
        halt();
    }

    // FETCH: Muestra la información del sistema
    else if (strcmp(command_buffer, "fetch") == 0) {
		print(" _    ___   ____   _  _   \n", 0x0B);
        print("/ |  / _ \\ |___ \\ | || |  \n", 0x0B);
        print("| | | | | |  __) || || |_ \n", 0x0B);
        print("| | | |_| | / __/ |__   _|\n", 0x0B);
        print("|_|  \\___/ |_____|   |_|  \n", 0x0B);
        print("--------------------------------------\n", 0x07);
        print("OS:            1024OS v0.4-rc1\n", 0x0F);
        print("Kernel:        Pavilionix86 0.2\n", 0x0F);
        print("Shell:         mini-sh 0.1\n", 0x0F);
	    print("Init System:   dvInit 0.2\n", 0x0F);
	    print("Arch:          i386 (x86)\n", 0x0F);
    }

    // CREDITS: Muestra los creditos del sistema
    else if (strcmp(command_buffer, "credits") == 0) {
	    print("Hecho por: Andresqwq\n", 0x0B);
	    print("Gracias por probar 1024OS!\n", 0x0F);
	    beep(50);
    }

    // WHOAMI: Muestra quien eres
    else if (strcmp(command_buffer, "whoami") == 0) {
	print("root\n", 0x0F);
    }

    // ECHO : Imprime lo que escribes después de "echo "
    else if (strncmp(command_buffer, "echo ", 5) == 0) {
        // Imprimimos desde la posición 5 del búfer (después de "echo ")
        print(&command_buffer[5], 0x07);
        print("\n", 0x07);
    }
    else if (strcmp(command_buffer, "echo") == 0) {
        print("Uso: echo <mensaje>\n", 0x0E);
    }

    // CHANGELOG: Muestra los cambios de las versiones
    else if (strcmp(command_buffer, "changelog") == 0) {
	print("0.1:\n", 0x0A);
	print("- Primera version!!!\n", 0x0F);

    print("0.2:\n", 0x0A);
    print("- Reemplazado: Se elimino el bootloader GRUB por Syslinux.\n", 0x0F);
    print("- Corregido: Se corrigieron algunos bugs menores.\n", 0x0F);
    print("- Corregido: Se corrigio un bug de que el texto se quedaba sin pantalla.\n", 0x0F);
    print("- Corregido: Se reescribio el codigo y ahora es mas estable y ordenado.\n", 0x0F);
    print("- Añadido: Mas dispositivos compatible con poweroff.\n", 0x0F);
    print("- Añadido: Soporte para mayusculas.\n", 0x0F);
	print("0.3:\n", 0x0A);

	print("- Reemplazado: Se reemplazo el script de build, por uno mas robusto, mas\ndetallado, y mejor estructurado.\n", 0x0F);
	print("- Corregido: Arte ascii de 'fetch' ahora esta bien hecho.\n", 0x0F);
	print("- Corregido: Error al identificar la version de el sistema.\n", 0x0F);
	print("- Eliminado: Eliminado el comando 'matrix', se planea añadir uno mejor\nimplementado pronto\n", 0x0F);

    print("0.4-rc1:\n", 0x0A);
    print("- Corregido: Reestructuracion de codigo fuente");
    print("Nota: Cada herramienta, se actualiza independientemente de las demás.\n", 0x0E);
    }

    // FIN DE COMANDOS


    // Si el comando no es reconocido y el buffer no está vacío, mostramos un error
    else if (buffer_idx > 0) {
        print("Comando desconocido.\n", 0x0C);
    }
    // Solo imprimimos el prompt si no limpiamos la pantalla
    if (strcmp(command_buffer, "clear") != 0) {
        print("> ", 0x07);
    }
    buffer_idx = 0;
}

// ---------------------
// /* --- TECLADO --- */
// ---------------------

char get_ascii(unsigned char sc, int shift) {
    static char map_normal[128] = {
        0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 0, 0, 0, 0,
        'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', 0, 0, 0, 0,
        'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 0, 0, 0, 0, 0,
        'z', 'x', 'c', 'v', 'b', 'n', 'm'
    };
    
    static char map_shift[128] = {
        0, 0, '!', '"', '#', '$', '%', '&', '/', '(', ')', '=', 0, 0, 0, 0,
        'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', 0, 0, 0, 0,
        'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 0, 0, 0, 0, 0,
        'Z', 'X', 'C', 'V', 'B', 'N', 'M'
    };

    if (sc == 0x39) return ' '; // Espacio
    if (sc < 128) {
        return shift ? map_shift[sc] : map_normal[sc];
    }
    return 0;
}

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
    print("Version: 0.4-rc1\n", 0x0F);
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
