# System Call Interface (Syscall Assembly)
# Fast system call entry point using SYSCALL/SYSENTER

.global syscall_entry
.global sysenter_entry
.global syscall_return
.global sys_exit
.global sys_write
.global sys_read

# SYSCALL entry point (AMD64)
# Registers: RAX=syscall#, RDI, RSI, RDX, R10, R8, R9=args
.global syscall_entry
syscall_entry:
    # Save user registers
    mov %rcx, -8(%rsp)          # Save RIP
    mov %r11, -16(%rsp)         # Save RFLAGS
    
    # Validate syscall number
    cmp $256, %rax              # Max syscalls
    jge syscall_invalid
    
    # Dispatch syscall
    lea syscall_table(%rip), %rcx
    mov (%rcx, %rax, 8), %rcx
    call *%rcx
    
    # Return to user space
    jmp syscall_return
    
syscall_invalid:
    mov $-1, %rax
    
# SYSENTER entry point (Intel)
.global sysenter_entry
sysenter_entry:
    # Similar but uses SYSENTER instruction
    ret

# Return from syscall
.global syscall_return
syscall_return:
    # RDX:RAX contains return value
    sysretq

# Syscall: exit
.global sys_exit
sys_exit:
    # RDI = exit code
    mov $0x3c, %rax             # exit syscall number
    jmp syscall_return

# Syscall: write
.global sys_write
sys_write:
    # RDI = fd, RSI = buf, RDX = len
    mov $0x01, %rax             # write syscall number
    
    # Call C handler
    lea handle_write(%rip), %rcx
    call *%rcx
    
    jmp syscall_return

# Syscall: read
.global sys_read
sys_read:
    # RDI = fd, RSI = buf, RDX = len
    mov $0x00, %rax             # read syscall number
    
    # Call C handler
    lea handle_read(%rip), %rcx
    call *%rcx
    
    jmp syscall_return

# Syscall dispatch table
.align 8
syscall_table:
    .quad handle_read           # 0: read
    .quad handle_write          # 1: write
    .quad 0                     # 2: open
    .quad 0                     # 3: close
    # ... more syscalls
