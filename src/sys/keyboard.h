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
