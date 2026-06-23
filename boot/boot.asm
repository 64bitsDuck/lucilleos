default abs
[org 0x7c00]
bits 16

_start:
    ; Clear segment registers for a known execution state
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00

    ; Print '1' to confirm 16-bit mode
    mov ah, 0x0e
    mov al, '1'
    int 0x10

    ; Read Kernel from Disk into Memory Address 0x8000
    mov ah, 0x02
    mov al, 8
    mov ch, 0
    mov dh, 0
    mov cl, 2
    mov bx, 0x8000
    int 0x13
    jc disk_error

    ; Proceed to 32-bit Protected Mode Switch
    cli
    lgdt [gdt_descriptor]

    mov eax, cr0
    or eax, 0x1
    mov cr0, eax

    jmp CODE_SEG:init_pm

disk_error:
    mov ah, 0x0e
    mov al, 'E'
    int 0x10
    jmp halt

bits 32
init_pm:
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax

    mov byte [0xb8002], '2'

    ; Clear page table space (0x1000 to 0x4000)
    mov edi, 0x1000
    mov cr3, edi
    xor eax, eax
    mov ecx, 3072
    rep stosd
    mov edi, cr3

    ; Build PML4 and PDPT
    mov dword [edi], 0x2003
    mov dword [edi + 0x1000], 0x3003
    
    ; Map 2MB identity huge page in PD
    mov dword [edi + 0x2000], 0x83

    ; Enable PAE
    mov eax, cr4
    or eax, 0x20
    mov cr4, eax

    ; Enable Long Mode in EFER MSR
    mov ecx, 0xc0000080
    rdmsr
    or eax, 0x100
    wrmsr

    ; Enable Paging
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax

    lgdt [gdt64_descriptor]
    jmp CODE64_SEG:init_lm

bits 64
init_lm:
    mov byte [0xb8004], '3'

    ; Jump directly into our loaded kernel code at 0x8000!
    jmp 0x8000

halt:
    hlt
    jmp halt

align 8
gdt_start:
    dq 0x0
gdt_code:
    dw 0xffff, 0x0000
    db 0x00, 0x9a, 0xcf, 0x00
gdt_data:
    dw 0xffff, 0x0000
    db 0x00, 0x92, 0xcf, 0x00
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

gdt64_start:
    dq 0x0
gdt64_code:
    dq 0x00209a0000000000
gdt64_data:
    dq 0x0000920000000000
gdt64_end:

gdt64_descriptor:
    dw gdt64_end - gdt64_start - 1
    dd gdt64_start

CODE_SEG   equ gdt_code - gdt_start
DATA_SEG   equ gdt_data - gdt_start
CODE64_SEG equ gdt64_code - gdt64_start

times 510 - ($ - $$) db 0
dw 0xaa55