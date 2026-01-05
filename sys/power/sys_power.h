#ifndef SYS_POWER_H
#define SYS_POWER_H

#include <stdint.h>

typedef struct
{
    uint32_t power_state;
    uint32_t battery_level;
    uint32_t power_profile;
    uint32_t cpu_freq;
} sys_power_state_t;

typedef struct
{
    uint32_t total_power_domains;
    uint32_t active_domains;
    uint64_t total_power_consumption;
    uint32_t sleep_count;
} sys_power_info_t;

int sys_power_init(void);
int sys_power_set_state(uint32_t power_state);
int sys_power_get_battery_level(uint32_t *level);
int sys_power_set_power_profile(uint32_t profile);
int sys_power_set_cpu_freq(uint32_t freq);
int sys_power_sleep(uint32_t seconds);
int sys_power_get_info(sys_power_info_t *info);

#endif
