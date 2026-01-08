; System Call Implementation - Part 6 Security
; Syscall entry point and dispatch

global syscall_entry
global syscall_dispatch

extern syscall_handler

; SYSCALL instruction entry point
; Used by user programs to invoke system calls
syscall_entry:
    ; RAX contains syscall number
    ; RDI, RSI, RDX, RCX, R8, R9 contain arguments
    
    ; Save user context
    push rcx              ; Return address
    push r11              ; RFLAGS
    push rax              ; Syscall number
    
    ; Build syscall context on stack
    sub rsp, 56
    mov [rsp + 0], rdi    ; arg1
    mov [rsp + 8], rsi    ; arg2
    mov [rsp + 16], rdx   ; arg3
    mov [rsp + 24], rcx   ; arg4
    mov [rsp + 32], r8    ; arg5
    mov [rsp + 40], r9    ; arg6
    
    ; Get syscall number
    mov rdi, [rsp + 56]   ; rax was pushed
    mov rsi, rsp          ; pointer to args
    
    ; Call C syscall dispatcher
    call syscall_dispatch
    
    ; Return value in rax
    add rsp, 56
    
    ; Restore user context
    pop rax               ; Was syscall number, now contains result
    pop r11               ; RFLAGS
    pop rcx               ; Return address
    
    sysretq

; Dispatch syscall to handler
; RDI = syscall number
; RSI = pointer to arguments
syscall_dispatch:
    push rbx
    
    ; Check syscall number bounds
    cmp rdi, 83
    jge invalid_syscall
    
    ; Call appropriate handler based on syscall number
    cmp rdi, 0            ; SYS_read
    je sys_read
    
    cmp rdi, 1            ; SYS_write
    je sys_write
    
    cmp rdi, 2            ; SYS_open
    je sys_open
    
    cmp rdi, 3            ; SYS_close
    je sys_close
    
    ; Add more syscall handlers as needed
    
    jmp invalid_syscall

sys_read:
    ; RDI = fd, RSI = buffer, RDX = size
    mov rdi, [rsi + 0]
    mov rsi, [rsi + 8]
    mov rdx, [rsi + 16]
    call syscall_handler
    jmp syscall_done

sys_write:
    mov rdi, [rsi + 0]
    mov rsi, [rsi + 8]
    mov rdx, [rsi + 16]
    call syscall_handler
    jmp syscall_done

sys_open:
    mov rdi, [rsi + 0]
    mov rsi, [rsi + 8]
    mov rdx, [rsi + 16]
    call syscall_handler
    jmp syscall_done

sys_close:
    mov rdi, [rsi + 0]
    call syscall_handler
    jmp syscall_done

invalid_syscall:
    mov rax, -1
    jmp syscall_done

syscall_done:
    pop rbx
    ret
