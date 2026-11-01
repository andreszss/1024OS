#ifndef HARDWARE_H
#define HARDWARE_H

#include <sys/cpu.h>

// obtener nombre real del procesador
void get_cpu_name(char *buffer) {
    unsigned int eax, ebx, ecx, edx;
    
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
        get_cpu_vendor(buffer);
    }
}

// obtener memoria ram 
unsigned int get_total_ram_mb() {
    outb(0x70, 0x30);
    unsigned char low = inb(0x71);
    outb(0x70, 0x31);
    unsigned char high = inb(0x71);
    
    unsigned int kb_ext = (high << 8) | low;
    // 1MB base + Memoria Extendida en MB
    return 1 + (kb_ext / 1024);
}

// enteros a texto para impresión rapida sin printf
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

// uptime calculado con el reloj de tiempo real (RTC / CMOS)
struct rtc_time {
    unsigned char sec, min, hour, day, mon, year;
};

static unsigned int boot_time = 0;
static int boot_time_valid = 0;

static unsigned char rtc_read(unsigned char reg) {
    outb(0x70, reg);
    return inb(0x71);
}

static int rtc_updating(void) {
    return rtc_read(0x0A) & 0x80;   // bit 7 = actualizacion en curso
}

static unsigned char bcd_to_bin(unsigned char v) {
    return (v & 0x0F) + ((v >> 4) * 10);
}

static void rtc_read_raw(struct rtc_time *t) {
    while (rtc_updating());
    t->sec  = rtc_read(0x00);
    t->min  = rtc_read(0x02);
    t->hour = rtc_read(0x04);
    t->day  = rtc_read(0x07);
    t->mon  = rtc_read(0x08);
    t->year = rtc_read(0x09);
}

static int rtc_same(struct rtc_time *a, struct rtc_time *b) {
    return a->sec == b->sec && a->min == b->min && a->hour == b->hour &&
           a->day == b->day && a->mon == b->mon && a->year == b->year;
}

// dias desde 1970-01-01 (algoritmo days_from_civil, sin librerias)
static unsigned int days_from_civil(int y, int m, int d) {
    y -= (m <= 2);
    int era = y / 400;
    unsigned int yoe = y - era * 400;
    unsigned int doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
    unsigned int doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    return era * 146097 + doe - 719468;
}

// segundos desde 1970 segun el RTC (solo importa la diferencia)
static unsigned int rtc_seconds_now(void) {
    struct rtc_time a, b;
    do {                       // leer dos veces hasta que coincidan
        rtc_read_raw(&a);
        rtc_read_raw(&b);
    } while (!rtc_same(&a, &b));

    unsigned char regb = rtc_read(0x0B);
    int pm = a.hour & 0x80;
    a.hour &= 0x7F;

    if (!(regb & 0x04)) {      // los valores vienen en BCD
        a.sec  = bcd_to_bin(a.sec);
        a.min  = bcd_to_bin(a.min);
        a.hour = bcd_to_bin(a.hour);
        a.day  = bcd_to_bin(a.day);
        a.mon  = bcd_to_bin(a.mon);
        a.year = bcd_to_bin(a.year);
    }
    if (!(regb & 0x02)) {      // modo de 12 horas
        a.hour = (a.hour % 12) + (pm ? 12 : 0);
    }

    unsigned int days = days_from_civil(2000 + a.year, a.mon, a.day);
    return days * 86400u + a.hour * 3600u + a.min * 60u + a.sec;
}

// llamar una vez al inicio de kernel_main()
void uptime_init(void) {
    boot_time = rtc_seconds_now();
    boot_time_valid = 1;
}

static void uptime_append(char *buffer, int *idx, unsigned int n, char suffix, int last) {
    char tmp[12];
    itoa((int)n, tmp);
    for (int i = 0; tmp[i]; i++) buffer[(*idx)++] = tmp[i];
    buffer[(*idx)++] = suffix;
    if (!last) buffer[(*idx)++] = ' ';
}

// buffer: minimo 20 bytes
void get_uptime_string(char *buffer) {
    if (!boot_time_valid) uptime_init();

    unsigned int now = rtc_seconds_now();
    unsigned int total = (now >= boot_time) ? (now - boot_time) : 0;

    unsigned int days  = total / 86400u;
    unsigned int hours = (total / 3600u) % 24u;
    unsigned int mins  = (total / 60u) % 60u;
    unsigned int secs  = total % 60u;

    int idx = 0;
    if (days)  uptime_append(buffer, &idx, days,  'd', 0);
    if (days || hours) uptime_append(buffer, &idx, hours, 'h', 0);
    uptime_append(buffer, &idx, mins, 'm', 0);
    uptime_append(buffer, &idx, secs, 's', 1);
    buffer[idx] = '\0';
}

#endif
