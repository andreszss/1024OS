#ifndef SCREEN_H
#define SCREEN_H

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
        outb(0x43, 0x00);
        unsigned char laton = inb(0x40);
        unsigned char haton = inb(0x40);
    }
}

// Scroll
void scroll() {
    if (term_y >= 25) {
        for (int i = 0; i < 80 * 24 * 2; i++) {
            video_memory[i] = video_memory[i + 160];
        }

        for (int i = 80 * 24 * 2; i < 80 * 25 * 2; i += 2) {
            video_memory[i] = ' ';
            video_memory[i + 1] = 0x07;
        }

        term_y = 24;
    }
}

#endif