void kmain(void) {
    // Write 'C' directly to VGA memory
    volatile char* video_memory = (volatile char*)0xb8000;
    
    video_memory[8] = 'C';
    video_memory[9] = 0x0f;

    while(1) {
        // Halt the CPU inside a loop to prevent it from burning power
        __asm__ volatile("hlt");
    }
}