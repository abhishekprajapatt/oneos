#ifndef ARCH_X86_LIB_H
#define ARCH_X86_LIB_H

#include <stdint.h>

typedef struct
{
    void (*delay_func)(uint32_t microseconds);
    void (*halt_func)(void);
    void (*reboot_func)(void);
} x86_lib_operations_t;

typedef struct
{
    uint32_t cpu_flags;
    uint32_t cache_line_size;
    uint32_t page_size;
} x86_lib_config_t;

int x86_lib_init(void);
int x86_lib_delay(uint32_t microseconds);
int x86_lib_halt_cpu(void);
int x86_lib_reboot_system(void);
int x86_lib_get_cpuid(uint32_t feature_id, uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx);
int x86_lib_enable_interrupts(void);
int x86_lib_disable_interrupts(void);
int x86_lib_get_config(x86_lib_config_t *config);

#endif
