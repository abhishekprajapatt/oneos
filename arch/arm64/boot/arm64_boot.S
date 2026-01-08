; ARM64 Boot - Part 3 Architecture Support
; ARM 64-bit architecture initialization

.global _start
.global enable_mmu
.global setup_gic

_start:
    ; Set up exception level
    mrs x0, currentel
    and x0, x0, #0xC
    cmp x0, #0xC
    b.eq el3_entry
    
    cmp x0, #0x8
    b.eq el2_entry
    
    cmp x0, #0x4
    b.eq el1_entry

el3_entry:
    ; Initialize at EL3
    mov x0, #0x531
    msr scr_el3, x0
    
    ; Drop to EL2
    adr x0, el2_entry
    msr elr_el3, x0
    eret

el2_entry:
    ; Initialize at EL2
    mov x0, #0x5B1
    msr hcr_el2, x0
    
    ; Drop to EL1
    adr x0, el1_entry
    msr elr_el2, x0
    eret

el1_entry:
    ; Initialize at EL1
    adr sp, stack_top
    
    ; Call C kernel main
    bl kernel_main
    
    ; Halt
    wfi
    b el1_entry

; Enable Memory Management Unit
enable_mmu:
    ; x0 = page table address
    msr ttbr0_el1, x0
    
    ; Set TCR
    mov x0, #(25 << 0)
    msr tcr_el1, x0
    
    ; Set MAIR
    mov x0, #0xFF
    msr mair_el1, x0
    
    ; Enable MMU
    mrs x0, sctlr_el1
    orr x0, x0, #0x1
    msr sctlr_el1, x0
    
    ; Synchronize
    dsb sy
    isb
    
    ret

; Setup GIC (Generic Interrupt Controller)
setup_gic:
    ; x0 = GIC distributor address
    ; x1 = GIC CPU interface address
    
    ; Initialize distributor
    mov w2, #0x1
    str w2, [x0, #0x0]
    
    ; Initialize CPU interface
    mov w2, #0x1
    str w2, [x1, #0x0]
    
    ret

.section .bss
stack_bottom:
    .space 0x1000
stack_top:
