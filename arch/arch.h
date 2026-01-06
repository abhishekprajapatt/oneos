#ifndef ARCH_H
#define ARCH_H

#include <stdint.h>

typedef struct
{
    uint32_t version;
    uint32_t cpu_count;
    uint32_t page_size;
} arch_info_t;

int arch_init(void);
int arch_get_info(arch_info_t *info);
int arch_setup_memory(uint32_t base, uint32_t size);
int arch_setup_interrupts(void);
int arch_start_cpu(uint32_t cpu_id);

#endif
