# ARM64 Bootstrap Code
# Initializes CPU from EL3 to EL1 (kernel mode)

.global _start
.global context_switch_arm64
.global arm64_cpu_halt
.global enable_mmu
.global disable_mmu
.global exception_handler

# Exception vector table
.align 11
exception_vectors:
    # Current EL with SP0
    .align 7
    sync_handler_sp0
    .align 7
    irq_handler_sp0
    .align 7
    fiq_handler_sp0
    .align 7
    serror_handler_sp0
    
    # Current EL with SPx
    .align 7
    sync_handler_spx
    .align 7
    irq_handler_spx
    .align 7
    fiq_handler_spx
    .align 7
    serror_handler_spx
    
    # Lower EL using AArch64
    .align 7
    sync_handler_64
    .align 7
    irq_handler_64
    .align 7
    fiq_handler_64
    .align 7
    serror_handler_64
    
    # Lower EL using AArch32
    .align 7
    sync_handler_32
    .align 7
    irq_handler_32
    .align 7
    fiq_handler_32
    .align 7
    serror_handler_32

# Entry point
.section .text.boot
_start:
    # Disable MMU and caches
    mrs x0, sctlr_el1
    bic x0, x0, #0x1            # Disable MMU
    bic x0, x0, #0x4            # Disable data cache
    msr sctlr_el1, x0
    isb
    
    # Set exception vector base address
    adr x0, exception_vectors
    msr vbar_el1, x0
    isb
    
    # Initialize stack pointer
    adr x0, _stack_end
    mov sp, x0
    
    # Jump to kernel
    bl kernel_main
    
    # Should not return
    b arm64_cpu_halt

# Enable MMU
# void enable_mmu(void)
.global enable_mmu
enable_mmu:
    mrs x0, sctlr_el1
    orr x0, x0, #0x1            # Enable MMU
    orr x0, x0, #0x4            # Enable data cache
    orr x0, x0, #0x1000         # Enable instruction cache
    msr sctlr_el1, x0
    isb
    ret

# Disable MMU
# void disable_mmu(void)
.global disable_mmu
disable_mmu:
    mrs x0, sctlr_el1
    bic x0, x0, #0x1            # Disable MMU
    bic x0, x0, #0x4            # Disable data cache
    bic x0, x0, #0x1000         # Disable instruction cache
    msr sctlr_el1, x0
    isb
    ret

# Context switch - ARM64
# void context_switch_arm64(struct arm64_context *new, struct arm64_context *old)
.global context_switch_arm64
context_switch_arm64:
    # x0 = new context
    # x1 = old context
    
    # Save current context to old
    stp x2, x3, [x1, #0]
    stp x4, x5, [x1, #16]
    stp x6, x7, [x1, #32]
    stp x8, x9, [x1, #48]
    stp x10, x11, [x1, #64]
    stp x12, x13, [x1, #80]
    stp x14, x15, [x1, #96]
    stp x16, x17, [x1, #112]
    stp x18, x19, [x1, #128]
    stp x20, x21, [x1, #144]
    stp x22, x23, [x1, #160]
    stp x24, x25, [x1, #176]
    stp x26, x27, [x1, #192]
    stp x28, x29, [x1, #208]
    str x30, [x1, #224]
    
    # Restore new context
    ldp x2, x3, [x0, #0]
    ldp x4, x5, [x0, #16]
    ldp x6, x7, [x0, #32]
    ldp x8, x9, [x0, #48]
    ldp x10, x11, [x0, #64]
    ldp x12, x13, [x0, #80]
    ldp x14, x15, [x0, #96]
    ldp x16, x17, [x0, #112]
    ldp x18, x19, [x0, #128]
    ldp x20, x21, [x0, #144]
    ldp x22, x23, [x0, #160]
    ldp x24, x25, [x0, #176]
    ldp x26, x27, [x0, #192]
    ldp x28, x29, [x0, #208]
    ldr x30, [x0, #224]
    
    ret

# Exception handlers
sync_handler_sp0:
irq_handler_sp0:
fiq_handler_sp0:
serror_handler_sp0:
sync_handler_spx:
irq_handler_spx:
fiq_handler_spx:
serror_handler_spx:
sync_handler_64:
irq_handler_64:
fiq_handler_64:
serror_handler_64:
sync_handler_32:
irq_handler_32:
fiq_handler_32:
serror_handler_32:
    stp x29, x30, [sp, #-16]!
    bl exception_handler
    ldp x29, x30, [sp], #16
    eret

# CPU halt
.global arm64_cpu_halt
arm64_cpu_halt:
    wfi                         # Wait for interrupt
    b arm64_cpu_halt

# Stack space
.align 12
_stack:
    .space 4096
_stack_end:
