; Interrupt Handler Entry Points - x86_64
; Part 1/2: Core kernel interrupt handling

global interrupt_handler
global irq_handler
global exception_handler
global syscall_entry

extern handle_exception
extern handle_interrupt
extern handle_syscall

; Generic interrupt handler
interrupt_handler:
    ; Save all general purpose registers
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    
    ; Get interrupt number from error code
    mov rdi, [rsp + 120]
    
    ; Call C handler
    call handle_interrupt
    
    ; Restore registers
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    
    ; Return from interrupt
    iretq

; IRQ Handler
irq_handler:
    ; Save context
    pushfq
    push rbp
    mov rbp, rsp
    
    ; Get IRQ number
    mov rdi, [rbp + 16]
    
    ; Call C IRQ handler
    call handle_interrupt
    
    ; Restore context
    pop rbp
    popfq
    
    iretq

; Exception Handler
exception_handler:
    push rax
    push rbx
    push rcx
    
    ; Error code in rsi
    mov rsi, [rsp + 24]
    
    ; Call exception handler
    call handle_exception
    
    pop rcx
    pop rbx
    pop rax
    
    iretq

; System Call Entry Point (syscall instruction)
syscall_entry:
    ; rax = syscall number
    ; rdi, rsi, rdx, rcx, r8, r9 = arguments
    
    ; Save user context
    push rcx
    push r11
    
    ; Switch to kernel stack
    mov r10, rsp
    mov rsp, [syscall_kernel_stack]
    
    ; Call C syscall handler
    ; rdi = syscall number, rsi = arguments pointer
    mov rsi, r10
    call handle_syscall
    
    ; Return value in rax
    
    ; Return to user mode
    pop r11
    pop rcx
    sysretq

; Data section
syscall_kernel_stack:
    dq 0x0
