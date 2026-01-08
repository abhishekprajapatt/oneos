# Virtualization Assembly Support
# CPU instructions for hypervisor and guest operations

.global vmx_on
.global vmx_off
.global vmlaunch
.global vmresume
.global vmexit_handler
.global cpu_has_vmx
.global cpu_has_svm
.global get_msr_asm
.global set_msr_asm

# Check CPU support for Intel VMX
# int cpu_has_vmx(void)
.global cpu_has_vmx
cpu_has_vmx:
    mov $1, %eax
    cpuid
    shr $5, %ecx
    and $1, %ecx
    mov %ecx, %eax
    ret

# Check CPU support for AMD SVM
# int cpu_has_svm(void)
.global cpu_has_svm
cpu_has_svm:
    mov $0x80000001, %eax
    cpuid
    shr $2, %ecx
    and $1, %ecx
    mov %ecx, %eax
    ret

# Enable VMX
# int vmx_on(uint64_t vmxon_ptr)
.global vmx_on
vmx_on:
    # rdi = VMXON region pointer
    
    # Set CR4.VMXE
    mov %cr4, %rax
    or $0x2000, %rax
    mov %rax, %cr4
    
    # VMXON instruction
    vmxon (%rdi)
    
    jnc vmx_on_success
    mov $-1, %eax
    ret
    
vmx_on_success:
    xor %eax, %eax
    ret

# Disable VMX
# void vmx_off(void)
.global vmx_off
vmx_off:
    vmxoff
    
    # Clear CR4.VMXE
    mov %cr4, %rax
    and $~0x2000, %rax
    mov %rax, %cr4
    
    ret

# Launch VM
# int vmlaunch(void)
.global vmlaunch
vmlaunch:
    vmlaunch
    
    jnc vmlaunch_success
    mov $-1, %eax
    ret
    
vmlaunch_success:
    xor %eax, %eax
    ret

# Resume VM
# int vmresume(void)
.global vmresume
vmresume:
    vmresume
    
    jnc vmresume_success
    mov $-1, %eax
    ret
    
vmresume_success:
    xor %eax, %eax
    ret

# VM exit handler
.global vmexit_handler
vmexit_handler:
    # Save guest state
    # Handle exit
    # Return to guest
    
    ret

# Read MSR
# uint64_t get_msr_asm(uint32_t msr)
.global get_msr_asm
get_msr_asm:
    # edi = MSR index
    mov %edi, %ecx
    rdmsr
    shl $32, %rdx
    or %rdx, %rax
    ret

# Write MSR
# void set_msr_asm(uint32_t msr, uint64_t value)
.global set_msr_asm
set_msr_asm:
    # edi = MSR index, rsi = value
    mov %edi, %ecx
    mov %rsi, %rax
    mov %rsi, %rdx
    shr $32, %rdx
    wrmsr
    ret
