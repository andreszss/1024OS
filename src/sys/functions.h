
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
