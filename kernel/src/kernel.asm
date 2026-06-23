default abs
bits 64
global _start
extern kmain

_start:
    mov rsp, 0x90000
    
    call kmain

inf_loop:
    hlt
    jmp inf_loop