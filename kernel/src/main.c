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

    // Shift rows 1-24 up to 0-23
    for (int y = 1; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            int src = ((y * VGA_WIDTH) + x) * 2;
            int dst = (((y - 1) * VGA_WIDTH) + x) * 2;
            vga[dst] = vga[src];
            vga[dst + 1] = vga[src + 1];
        }
    }

    // Clear bottom row
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

void kmain(void) {
    kclear();

    kprint("Welcome to LucilleOS!\n");
    kprint("---------------------\n");
    kprint("Status: 64-bit Long Mode Active\n");
    kprint("Driver: VGA Text Screen Initialization... SUCCESS\n");
    
    // Test loop to force scroll validation
    for (int i = 0; i < 30; i++) {
        kprint("Testing terminal scroll line allocation...\n");
    }
}