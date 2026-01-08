# RISC-V Boot - Part 3 Architecture Support
# RISC-V 64-bit architecture initialization

.global _start
.global enable_paging
.global setup_interrupts

_start:
    # Initialize stack
    la sp, stack_top
    
    # Get hart ID
    csrr a0, mhartid
    
    # Call kernel main
    call kernel_main
    
    # Halt
    wfi
    j _start

# Enable paging (SV39 mode)
enable_paging:
    # a0 = page table address
    
    # Configure SATP (Supervisor Address Translation and Protection)
    # Set mode to SV39 (mode 8)
    li t0, 8
    slli t0, t0, 60
    
    # Or with page table address
    or t0, t0, a0
    csrw satp, t0
    
    # Synchronize
    sfence.vma
    
    ret

# Setup interrupts
setup_interrupts:
    # Enable machine interrupts
    li t0, 0x888
    csrw mie, t0
    
    # Set interrupt handler address
    la t0, interrupt_handler
    csrw mtvec, t0
    
    # Enable global interrupts
    li t0, 0x8
    csrw mstatus, t0
    
    ret

.section .bss
stack_bottom:
    .space 0x1000
stack_top:
