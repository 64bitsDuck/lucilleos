#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_ADDRESS 0xb8000
#define DEFAULT_COLOR 0x0f

static int cursor_x = 0;
static int cursor_y = 0;

void poke_char(int x, int y, char c, char attribute) {
    volatile char* vga = (volatile char*)VGA_ADDRESS;
    int offset = ((y * VGA_WIDTH) + x) * 2;
    vga[offset] = c;
    vga[offset + 1] = attribute;
}

void kclear(void) {
    for (int y = 0; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            poke_char(x, y, ' ', DEFAULT_COLOR);
        }
    }
    cursor_x = 0;
    cursor_y = 0;
}

void kscroll(void) {
    volatile char* vga = (volatile char*)VGA_ADDRESS;

    for (int y = 1; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            int src = ((y * VGA_WIDTH) + x) * 2;
            int dst = (((y - 1) * VGA_WIDTH) + x) * 2;
            vga[dst] = vga[src];
            vga[dst + 1] = vga[src + 1];
        }
    }

    for (int x = 0; x < VGA_WIDTH; x++) {
        poke_char(x, VGA_HEIGHT - 1, ' ', DEFAULT_COLOR);
    }

    cursor_y = VGA_HEIGHT - 1;
}

void kprint_char(char c) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
        if (cursor_y >= VGA_HEIGHT) {
            kscroll();
        }
        return;
    }

    poke_char(cursor_x, cursor_y, c, DEFAULT_COLOR);
    cursor_x++;

    if (cursor_x >= VGA_WIDTH) {
        cursor_x = 0;
        cursor_y++;
    }

    if (cursor_y >= VGA_HEIGHT) {
        kscroll();
    }
}

void kprint(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        kprint_char(str[i]);
    }
}

void kprint_hex(unsigned long long val) {
    char hex_chars[] = "0123456789ABCDEF";
    char buffer[19];
    int pos = 0;

    buffer[pos++] = '0';
    buffer[pos++] = 'x';

    // Parse bits from most to least significant
    for (int i = 60; i >= 0; i -= 4) {
        int nibble = (val >> i) & 0xF;
        // Skip leading zeros except for the final digit
        if (pos > 2 || nibble != 0 || i == 0) {
            buffer[pos++] = hex_chars[nibble];
        }
    }
    buffer[pos] = '\0';
    kprint(buffer);
}

void kprint_int(long long val) {
    char buffer[32];
    int pos = 0;

    if (val == 0) {
        kprint_char('0');
        return;
    }

    if (val < 0) {
        kprint_char('-');
        val = -val;
    }

    while (val > 0) {
        buffer[pos++] = '0' + (val % 10);
        val /= 10;
    }

    // Print buffer in reverse order
    for (int i = pos - 1; i >= 0; i--) {
        kprint_char(buffer[i]);
    }
}

void kmain(void) {
    kclear();

    kprint("Welcome to LucilleOS!\n");
    kprint("---------------------\n");
    
    // Validate formatting capabilities
    kprint("Testing Integer: ");
    kprint_int(1234567890);
    kprint("\n");

    kprint("Testing Negative Integer: ");
    kprint_int(-42);
    kprint("\n");

    kprint("Testing Kernel Entry Pointer: ");
    kprint_hex(0x8000);
    kprint("\n");

    kprint("Testing Max 64-bit Hex Value: ");
    kprint_hex(0xFFFFFFFFFFFFFFFF);
    kprint("\n");

    while(1) {
        __asm__ volatile("hlt");
    }
}