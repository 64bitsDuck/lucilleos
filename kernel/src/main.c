#include "limine.h"
#include <stddef.h>
#include <stdint.h>

// Ask the bootloader for a framebuffer
__attribute__((used, section(".requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

// Halt the CPU
static void halt(void) {
    while (1) {
        __asm__("hlt");
    }
}

// Kernel entry point
void _start(void) {
    // Basic check that the bootloader responded to our request
    if (framebuffer_request.response == NULL || framebuffer_request.response->framebuffer_count < 1) {
        halt();
    }

    // Our first pixel! (Will be coded in the next step)
    halt();
}