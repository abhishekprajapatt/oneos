# Boot Loader Assembly
# Entry point for system boot process

.global _boot_entry
.global load_kernel
.global detect_cpu
.global setup_a20
.global switch_to_pm

.code16
_boot_entry:
    cli                         # Disable interrupts
    
    # Setup A20 line (enables access to memory > 1MB)
    call setup_a20
    
    # Detect CPU capabilities
    call detect_cpu
    
    # Load kernel
    call load_kernel
    
    # Should not return
    jmp .

# Setup A20 address line
setup_a20:
    mov $0x2401, %ax
    lmsw %ax
    ret

# Detect CPU
detect_cpu:
    mov $1, %eax
    cpuid
    # EAX = family/model/stepping
    # EDX = features
    ret

# Load kernel from disk
load_kernel:
    # Simple disk read routine
    mov $0x80, %dl              # Drive (first hard disk)
    mov $1, %al                 # Read 1 sector
    mov $2, %cl                 # Starting sector
    mov $0x1000, %bx            # Destination address
    int $0x13                   # BIOS disk interrupt
    ret

# Switch to protected mode
switch_to_pm:
    mov %cr0, %eax
    or $1, %eax
    mov %eax, %cr0
    jmp $0x08, $pm_start

.code32
pm_start:
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss
    ret
