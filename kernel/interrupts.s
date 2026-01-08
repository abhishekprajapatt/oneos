# Generic Interrupt Handling Code
# This file provides common interrupt handling utilities

.global irq_enter
.global irq_exit
.global isr_common
.global disable_interrupts_asm
.global enable_interrupts_asm

# Enter interrupt context
.global irq_enter
irq_enter:
    push %rbx
    push %rcx
    push %rdx
    push %rsi
    push %rdi
    push %r8
    push %r9
    push %r10
    push %r11
    
    mov %rsp, %rdi              # Pass stack pointer as first arg
    ret

# Exit interrupt context
.global irq_exit
irq_exit:
    pop %r11
    pop %r10
    pop %r9
    pop %r8
    pop %rdi
    pop %rsi
    pop %rdx
    pop %rcx
    pop %rbx
    
    iretq

# Common ISR handler
.global isr_common
isr_common:
    call irq_handler
    jmp irq_exit

# Disable interrupts (inline assembly wrapper)
# void disable_interrupts_asm(void)
.global disable_interrupts_asm
disable_interrupts_asm:
    cli
    ret

# Enable interrupts (inline assembly wrapper)
# void enable_interrupts_asm(void)
.global enable_interrupts_asm
enable_interrupts_asm:
    sti
    ret
