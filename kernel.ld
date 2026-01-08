OUTPUT_FORMAT("elf64-x86-64")
OUTPUT_ARCH(i386:x86-64)
ENTRY(_start)

SECTIONS
{
    . = 0xFFFFFFFF80000000;
    
    .boot : AT(ADDR(.boot) - 0xFFFFFFFF80000000)
    {
        *(.boot)
        *(.text.boot)
    }
    
    .text : AT(ADDR(.text) - 0xFFFFFFFF80000000)
    {
        *(.text)
        *(.text.*)
        *(.gnu.linkonce.t.*)
    }
    
    .rodata : AT(ADDR(.rodata) - 0xFFFFFFFF80000000)
    {
        *(.rodata)
        *(.rodata.*)
        *(.gnu.linkonce.r.*)
    }
    
    .ctors : AT(ADDR(.ctors) - 0xFFFFFFFF80000000)
    {
        KEEP(*(.ctors))
    }
    
    .dtors : AT(ADDR(.dtors) - 0xFFFFFFFF80000000)
    {
        KEEP(*(.dtors))
    }
    
    .data : AT(ADDR(.data) - 0xFFFFFFFF80000000)
    {
        *(.data)
        *(.data.*)
        *(.gnu.linkonce.d.*)
    }
    
    .tdata : AT(ADDR(.tdata) - 0xFFFFFFFF80000000)
    {
        *(.tdata)
        *(.tdata.*)
    }
    
    .tbss : AT(ADDR(.tbss) - 0xFFFFFFFF80000000)
    {
        *(.tbss)
        *(.tbss.*)
    }
    
    .bss : AT(ADDR(.bss) - 0xFFFFFFFF80000000)
    {
        *(.bss)
        *(.bss.*)
        *(COMMON)
    }
    
    .heap : AT(ADDR(.heap) - 0xFFFFFFFF80000000)
    {
        . += 0x100000;
    }
    
    .stack : AT(ADDR(.stack) - 0xFFFFFFFF80000000)
    {
        . += 0x200000;
    }
    
    /DISCARD/ :
    {
        *(.gnu.hash)
        *(.gnu.version)
        *(.gnu.version_r)
        *(.gnu_debuglink)
        *(.gnu_debugaltlink)
        *(.note.GNU-stack)
    }
    
    . = ALIGN(0x1000);
    _kernel_end = .;
}
