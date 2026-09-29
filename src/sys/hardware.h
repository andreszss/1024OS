#ifndef HARDWARE_H
#define HARDWARE_H

#include <sys/cpu.h>

// 1. OBTENER NOMBRE REAL DEL PROCESADOR (CPUID Brand String)
void get_cpu_name(char *buffer) {
    unsigned int eax, ebx, ecx, edx;
    
    // Comprobar si CPUID soporta las funciones extendidas
    asm volatile ("cpuid" : "=a"(eax) : "a"(0x80000000));
    
    if (eax >= 0x80000004) {
        unsigned int *ptr = (unsigned int*)buffer;
        for (unsigned int i = 0; i < 3; i++) {
            asm volatile ("cpuid"
                          : "=a"(ptr[i*4]), "=b"(ptr[i*4+1]), "=c"(ptr[i*4+2]), "=d"(ptr[i*4+3])
                          : "a"(0x80000002 + i));
        }
        buffer[48] = '\0';
    } else {
        // Fallback si no soporta la cadena completa
        get_cpu_vendor(buffer);
    }
}

// 2. OBTENER MEMORIA RAM REAL (Consulta al CMOS)
unsigned int get_total_ram_mb() {
    outb(0x70, 0x30);
    unsigned char low = inb(0x71);
    outb(0x70, 0x31);
    unsigned char high = inb(0x71);
    
    unsigned int kb_ext = (high << 8) | low;
    // 1MB base + Memoria Extendida en MB
    return 1 + (kb_ext / 1024);
}

// Convierte enteros a texto para impresión rápida sin printf
void itoa(int n, char *str) {
    int i = 0, is_neg = 0;
    if (n < 0) { is_neg = 1; n = -n; }
    if (n == 0) { str[i++] = '0'; str[i] = '\0'; return; }
    while (n != 0) {
        int rem = n % 10;
        str[i++] = rem + '0';
        n = n / 10;
    }
    if (is_neg) str[i++] = '-';
    str[i] = '\0';
    // Invertir cadena
    for (int j = 0; j < i / 2; j++) {
        char temp = str[j];
        str[j] = str[i - j - 1];
        str[i - j - 1] = temp;
    }
}

// 3. UPTIME (Basado en el reloj de sistema/ticks PIT)
void get_uptime_string(char *buffer) {
    // El timer del PIT vibra aprox. 18.2 veces por segundo por defecto en PC
    // Estimación mediante lectura de puerto 0x40 / contador
    static unsigned int ticks = 0;
    ticks++; 
    unsigned int total_seconds = ticks / 100; // Aproximación
    unsigned int mins = total_seconds / 60;
    unsigned int secs = total_seconds % 60;
    
    char m_str[10], s_str[10];
    itoa(mins, m_str);
    itoa(secs, s_str);
    
    int idx = 0;
    for (int i = 0; m_str[i]; i++) buffer[idx++] = m_str[i];
    buffer[idx++] = 'm'; buffer[idx++] = ' ';
    for (int i = 0; s_str[i]; i++) buffer[idx++] = s_str[i];
    buffer[idx++] = 's';
    buffer[idx] = '\0';
}

#endif