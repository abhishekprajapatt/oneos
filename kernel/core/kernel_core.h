#ifndef KERNEL_CORE_H
#define KERNEL_CORE_H

#include <stdint.h>

typedef struct
{
    uint32_t core_id;
    uint32_t cpu_freq;
    uint32_t load_percent;
    uint32_t status;
} kernel_core_info_t;

typedef struct
{
    uint32_t core_count;
    uint32_t active_cores;
    uint32_t idle_cores;
} kernel_core_state_t;

int kernel_core_init(void);
int kernel_core_get_info(uint32_t core_id, kernel_core_info_t *info);
int kernel_core_set_freq(uint32_t core_id, uint32_t freq);
int kernel_core_enable(uint32_t core_id);
int kernel_core_disable(uint32_t core_id);
int kernel_core_get_load(uint32_t core_id, uint32_t *load);

#endif
