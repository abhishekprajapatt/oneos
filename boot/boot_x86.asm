[BITS 64]
ORG 0x7C00

; OneOS Boot Loader - x86_64 Assembly
; Part 1: Foundation & Core

global _start
global gdt_descriptor
global idt_descriptor

_start:
    ; Initialize segment registers
    mov ax, 0x0000
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax
    
    ; Set up stack
    mov rsp, 0x9000
    mov rbp, 0x9000
    
    ; Load GDT
    lgdt [gdt_descriptor]
    
    ; Clear interrupt flag
    cli
    
    ; Enable 64-bit mode
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax
    
    ; Long jump to 64-bit code
    jmp 0x08:start64
    
[BITS 64]
start64:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax
    
    ; Call C kernel main
    call kernel_main
    
    ; Halt
    hlt
    jmp $

; Global Descriptor Table
align 16
gdt:
    dq 0x0000000000000000    ; Null descriptor
    dq 0x00209a0000000000    ; Code descriptor (64-bit)
    dq 0x0000920000000000    ; Data descriptor
    
gdt_descriptor:
    dw $ - gdt - 1
    dq gdt

; IDT Descriptor placeholder
idt_descriptor:
    dw 0x0FFF
    dq 0x0
