# Core Services Assembly Entry Points
# Process and thread management at assembly level

.global process_switch_entry
.global thread_spawn_entry
.global process_kill_entry
.global get_current_process
.global get_current_thread
.global save_registers
.global restore_registers

# Process context switching
# void process_switch_entry(process_t *next)
.global process_switch_entry
process_switch_entry:
    # rdi = next process
    
    # Save current registers
    call save_registers
    
    # Load next process registers
    call restore_registers
    
    ret

# Save all registers to process context
.global save_registers
save_registers:
    # Save to current process structure
    push %rax
    push %rbx
    push %rcx
    push %rdx
    push %rsi
    push %rdi
    push %rbp
    push %r8
    push %r9
    push %r10
    push %r11
    push %r12
    push %r13
    push %r14
    push %r15
    
    ret

# Restore registers from process context
.global restore_registers
restore_registers:
    # Restore from current process structure
    pop %r15
    pop %r14
    pop %r13
    pop %r12
    pop %r11
    pop %r10
    pop %r9
    pop %r8
    pop %rbp
    pop %rdi
    pop %rsi
    pop %rdx
    pop %rcx
    pop %rbx
    pop %rax
    
    ret

# Thread spawning
.global thread_spawn_entry
thread_spawn_entry:
    # rdi = thread entry point
    # rsi = thread args
    
    mov %rdi, %rax
    mov %rsi, %rdi
    call *%rax
    
    # Thread exit
    mov $0, %rax
    ret

# Kill process
.global process_kill_entry
process_kill_entry:
    # rdi = process ID
    
    # Clean up resources
    # Deallocate memory
    # Close file descriptors
    
    ret

# Get current process pointer
# process_t* get_current_process(void)
.global get_current_process
get_current_process:
    mov %gs:0x0, %rax           # Read from GS base (thread local)
    ret

# Get current thread pointer
# thread_t* get_current_thread(void)
.global get_current_thread
get_current_thread:
    mov %gs:0x8, %rax           # Read from GS base+8
    ret
