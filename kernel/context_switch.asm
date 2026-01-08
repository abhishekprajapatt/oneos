; Context Switch Implementation - x86_64
; Part 2: Process Management
; Context switching between processes/threads

global context_switch
global save_context
global restore_context

; rdi = from_context (pointer to context struct)
; rsi = to_context (pointer to context struct)
context_switch:
    ; Save current context (from_context)
    mov [rdi + 0],  rax
    mov [rdi + 8],  rbx
    mov [rdi + 16], rcx
    mov [rdi + 24], rdx
    mov [rdi + 32], rsi
    mov [rdi + 40], r8
    mov [rdi + 48], r9
    mov [rdi + 56], r10
    mov [rdi + 64], r11
    mov [rdi + 72], r12
    mov [rdi + 80], r13
    mov [rdi + 88], r14
    mov [rdi + 96], r15
    mov [rdi + 104], rbp
    
    ; Save stack pointer
    mov [rdi + 112], rsp
    
    ; Save flags and return address
    pushfq
    pop rax
    mov [rdi + 120], rax
    
    ; Load new context (to_context)
    mov rax, [rsi + 0]
    mov rbx, [rsi + 8]
    mov rcx, [rsi + 16]
    mov rdx, [rsi + 24]
    mov r8,  [rsi + 40]
    mov r9,  [rsi + 48]
    mov r10, [rsi + 56]
    mov r11, [rsi + 64]
    mov r12, [rsi + 72]
    mov r13, [rsi + 80]
    mov r14, [rsi + 88]
    mov r15, [rsi + 96]
    mov rbp, [rsi + 104]
    
    ; Restore stack pointer
    mov rsp, [rsi + 112]
    
    ; Restore flags
    mov rax, [rsi + 120]
    push rax
    popfq
    
    ret

; Save current CPU context
save_context:
    ; rdi = pointer to context buffer
    pushfq
    pop rax
    mov [rdi], rax
    
    mov [rdi + 8], rsp
    mov [rdi + 16], rbp
    mov [rdi + 24], rbx
    mov [rdi + 32], r12
    mov [rdi + 40], r13
    mov [rdi + 48], r14
    mov [rdi + 56], r15
    
    ret

; Restore CPU context
restore_context:
    ; rdi = pointer to context buffer
    mov rsp, [rdi + 8]
    mov rbp, [rdi + 16]
    mov rbx, [rdi + 32]
    mov r12, [rdi + 40]
    mov r13, [rdi + 48]
    mov r14, [rdi + 56]
    mov r15, [rdi + 64]
    
    mov rax, [rdi]
    push rax
    popfq
    
    ret
