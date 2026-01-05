#ifndef SYSTEM_H
#define SYSTEM_H

#include <stdint.h>

typedef struct
{
    const char *os_name;
    const char *os_version;
    uint32_t build_number;
    const char *system_uptime;
} system_info_t;

typedef struct
{
    uint64_t total_memory;
    uint64_t used_memory;
    uint64_t free_memory;
    uint32_t cpu_usage;
    uint32_t disk_usage;
} system_resources_t;

typedef struct
{
    uint64_t total_storage;
    uint64_t used_storage;
    uint64_t free_storage;
    uint32_t storage_usage_percent;
} storage_info_t;

int system_init(void);
int system_get_info(system_info_t *info);
int system_get_resources(system_resources_t *resources);
int system_get_storage_info(storage_info_t *storage);
int system_shutdown(void);
int system_reboot(void);
int system_sleep(void);
int system_wake(void);
int system_get_uptime(char *uptime, uint32_t max_len);

#endif
