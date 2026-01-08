# x86 Bootstrap Code
# Initializes CPU from real mode to protected mode

.code32
.global _start
.global gdt_flush
.global idt_flush
.global context_switch
.global interrupt_handler_entry
.global cpu_halt

# GDT descriptor table - loaded by bootloader
.align 8
gdt:
    .quad 0x0000000000000000    # NULL descriptor
    .quad 0x00cf9a000000ffff    # Kernel code segment
    .quad 0x00cf92000000ffff    # Kernel data segment
    .quad 0x00cffa000000ffff    # User code segment
    .quad 0x00cff2000000ffff    # User data segment
gdt_end:

gdt_ptr:
    .word gdt_end - gdt - 1     # GDT limit
    .long gdt                   # GDT base

# IDT pointer (interrupt descriptor table)
.align 8
idt_ptr:
    .word 0                     # IDT limit (will be set later)
    .long 0                     # IDT base (will be set later)

# Entry point
_start:
    cli                         # Disable interrupts
    
    # Load GDT
    lgdt gdt_ptr
    
    # Jump to kernel
    call kernel_main
    jmp .

# Load GDT
# void gdt_flush(uint32_t gdt_ptr)
.extern gdt_flush
gdt_flush:
    mov 4(%esp), %eax           # Get GDT ptr from stack
    lgdt (%eax)                 # Load GDT
    
    mov $0x10, %ax              # Load kernel data segment
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss
    
    ljmp $0x08, $gdt_flush_done # Jump to kernel code segment
    
gdt_flush_done:
    ret

# Load IDT
# void idt_flush(uint32_t idt_ptr)
.extern idt_flush
idt_flush:
    mov 4(%esp), %eax           # Get IDT ptr from stack
    lidt (%eax)                 # Load IDT
    ret

# Context switch - saves and restores CPU context
# void context_switch(struct cpu_context *new_ctx, struct cpu_context *old_ctx)
.extern context_switch
context_switch:
    push %ebp
    mov %esp, %ebp
    
    # Save current context
    mov 12(%ebp), %eax          # old_ctx pointer
    mov %eax, (%eax)            # Save EAX
    mov %ecx, 4(%eax)           # Save ECX
    mov %edx, 8(%eax)           # Save EDX
    mov %ebx, 12(%eax)          # Save EBX
    mov %esp, 16(%eax)          # Save ESP
    mov %ebp, 20(%eax)          # Save EBP
    mov %esi, 24(%eax)          # Save ESI
    mov %edi, 28(%eax)          # Save EDI
    
    # Restore new context
    mov 8(%ebp), %eax           # new_ctx pointer
    mov (%eax), %eax            # Restore EAX
    mov 4(%eax), %ecx           # Restore ECX
    mov 8(%eax), %edx           # Restore EDX
    mov 12(%eax), %ebx          # Restore EBX
    mov 16(%eax), %esp          # Restore ESP
    mov 20(%eax), %ebp          # Restore EBP
    mov 24(%eax), %esi          # Restore ESI
    mov 28(%eax), %edi          # Restore EDI
    
    ret

# Interrupt handler entry point
.extern interrupt_handler_entry
interrupt_handler_entry:
    pusha                       # Push all general purpose registers
    push %ds                    # Push data segment
    push %es                    # Push extra segment
    
    mov $0x10, %ax              # Load kernel data segment
    mov %ax, %ds
    mov %ax, %es
    
    # Call C interrupt handler
    call isr_handler
    
    pop %es                     # Pop extra segment
    pop %ds                     # Pop data segment
    popa                        # Pop all general purpose registers
    
    add $8, %esp                # Remove error code and interrupt number
    iret                        # Return from interrupt

# CPU halt
.extern cpu_halt
cpu_halt:
    hlt
    jmp cpu_halt

# Fill with NOPs
.fill 512 - (. - _start), 1, 0x90
