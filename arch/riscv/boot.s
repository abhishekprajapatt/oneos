# RISC-V Bootstrap Code
# Initializes CPU from machine mode to supervisor mode

.global _start
.global context_switch_riscv
.global riscv_cpu_halt
.global enable_paging
.global disable_paging
.global trap_handler

# Entry point
.section .text.boot
_start:
    # Initialize registers
    li sp, 0x80001000          # Set stack pointer
    
    # Disable interrupts
    csrc mstatus, MSTATUS_MIE
    
    # Set machine trap handler
    la t0, trap_handler
    csrw mtvec, t0
    
    # Initialize trap vector
    csrw mscratch, x0
    
    # Jump to kernel
    jal kernel_main
    
    # Should not return
    jal riscv_cpu_halt

# Enable paging (virtual memory)
# void enable_paging(void)
.global enable_paging
enable_paging:
    # Set page table root address
    # Assuming a1 contains PPN of root page table
    csrw satp, a0
    
    # Flush TLB
    sfence.vma
    
    ret

# Disable paging
# void disable_paging(void)
.global disable_paging
disable_paging:
    # Clear SATP to disable paging
    csrw satp, x0
    
    # Flush TLB
    sfence.vma
    
    ret

# Context switch - RISC-V
# void context_switch_riscv(struct riscv_context *new, struct riscv_context *old)
.global context_switch_riscv
context_switch_riscv:
    # a0 = new context
    # a1 = old context
    
    # Save current context to old (callee-saved registers)
    sw x1, 0(a1)                # ra (return address)
    sw x2, 4(a1)                # sp (stack pointer)
    sw x3, 8(a1)                # gp (global pointer)
    sw x4, 12(a1)               # tp (thread pointer)
    sw x8, 16(a1)               # s0 (frame pointer)
    sw x9, 20(a1)               # s1
    sw x18, 24(a1)              # s2
    sw x19, 28(a1)              # s3
    sw x20, 32(a1)              # s4
    sw x21, 36(a1)              # s5
    sw x22, 40(a1)              # s6
    sw x23, 44(a1)              # s7
    sw x24, 48(a1)              # s8
    sw x25, 52(a1)              # s9
    sw x26, 56(a1)              # s10
    sw x27, 60(a1)              # s11
    
    # Restore new context
    lw x1, 0(a0)                # ra
    lw x2, 4(a0)                # sp
    lw x3, 8(a0)                # gp
    lw x4, 12(a0)               # tp
    lw x8, 16(a0)               # s0
    lw x9, 20(a0)               # s1
    lw x18, 24(a0)              # s2
    lw x19, 28(a0)              # s3
    lw x20, 32(a0)              # s4
    lw x21, 36(a0)              # s5
    lw x22, 40(a0)              # s6
    lw x23, 44(a0)              # s7
    lw x24, 48(a0)              # s8
    lw x25, 52(a0)              # s9
    lw x26, 56(a0)              # s10
    lw x27, 60(a0)              # s11
    
    ret

# Trap handler
.global trap_handler
.align 4
trap_handler:
    # Save context
    addi sp, sp, -256
    sw x1, 0(sp)
    sw x2, 4(sp)
    sw x3, 8(sp)
    sw x4, 12(sp)
    sw x5, 16(sp)
    sw x6, 20(sp)
    sw x7, 24(sp)
    sw x8, 28(sp)
    sw x9, 32(sp)
    sw x10, 36(sp)
    sw x11, 40(sp)
    sw x12, 44(sp)
    sw x13, 48(sp)
    sw x14, 52(sp)
    sw x15, 56(sp)
    sw x16, 60(sp)
    sw x17, 64(sp)
    sw x18, 68(sp)
    sw x19, 72(sp)
    sw x20, 76(sp)
    sw x21, 80(sp)
    sw x22, 84(sp)
    sw x23, 88(sp)
    sw x24, 92(sp)
    sw x25, 96(sp)
    sw x26, 100(sp)
    sw x27, 104(sp)
    sw x28, 108(sp)
    sw x29, 112(sp)
    sw x30, 116(sp)
    sw x31, 120(sp)
    
    # Call trap handler in C
    jal handle_trap
    
    # Restore context
    lw x1, 0(sp)
    lw x2, 4(sp)
    lw x3, 8(sp)
    lw x4, 12(sp)
    lw x5, 16(sp)
    lw x6, 20(sp)
    lw x7, 24(sp)
    lw x8, 28(sp)
    lw x9, 32(sp)
    lw x10, 36(sp)
    lw x11, 40(sp)
    lw x12, 44(sp)
    lw x13, 48(sp)
    lw x14, 52(sp)
    lw x15, 56(sp)
    lw x16, 60(sp)
    lw x17, 64(sp)
    lw x18, 68(sp)
    lw x19, 72(sp)
    lw x20, 76(sp)
    lw x21, 80(sp)
    lw x22, 84(sp)
    lw x23, 88(sp)
    lw x24, 92(sp)
    lw x25, 96(sp)
    lw x26, 100(sp)
    lw x27, 104(sp)
    lw x28, 108(sp)
    lw x29, 112(sp)
    lw x30, 116(sp)
    lw x31, 120(sp)
    
    addi sp, sp, 256
    mret

# CPU halt
.global riscv_cpu_halt
riscv_cpu_halt:
    wfi                         # Wait for interrupt
    jal riscv_cpu_halt

# Stack space
.align 12
_stack:
    .space 4096
_stack_end:
