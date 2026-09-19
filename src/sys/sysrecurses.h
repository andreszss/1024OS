int term_x = 0; 
int term_y = 0;
int shift_pressed = 0;
int check_keystatus() {
    return inb(0x64) & 0x01;
}

unsigned char* video_memory = (unsigned char*)0xB8000; 
