# Driver Base Assembly
# Low-level I/O operations and device access

.global read_port_byte
.global read_port_word
.global read_port_dword
.global write_port_byte
.global write_port_word
.global write_port_dword
.global inb
.global inw
.global inl
.global outb
.global outw
.global outl

# Read byte from I/O port
# uint8_t read_port_byte(uint16_t port)
.global read_port_byte
read_port_byte:
    mov %edi, %edx
    xor %eax, %eax
    inb %dx, %al
    ret

# Read word from I/O port
# uint16_t read_port_word(uint16_t port)
.global read_port_word
read_port_word:
    mov %edi, %edx
    xor %eax, %eax
    inw %dx, %ax
    ret

# Read dword from I/O port
# uint32_t read_port_dword(uint16_t port)
.global read_port_dword
read_port_dword:
    mov %edi, %edx
    xor %eax, %eax
    inl %dx, %eax
    ret

# Write byte to I/O port
# void write_port_byte(uint16_t port, uint8_t value)
.global write_port_byte
write_port_byte:
    mov %edi, %edx
    mov %esi, %eax
    outb %al, %dx
    ret

# Write word to I/O port
# void write_port_word(uint16_t port, uint16_t value)
.global write_port_word
write_port_word:
    mov %edi, %edx
    mov %esi, %eax
    outw %ax, %dx
    ret

# Write dword to I/O port
# void write_port_dword(uint16_t port, uint32_t value)
.global write_port_dword
write_port_dword:
    mov %edi, %edx
    mov %esi, %eax
    outl %eax, %dx
    ret

# Aliases for common I/O operations
.global inb
inb = read_port_byte

.global inw
inw = read_port_word

.global inl
inl = read_port_dword

.global outb
outb = write_port_byte

.global outw
outw = write_port_word

.global outl
outl = write_port_dword
