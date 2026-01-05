#ifndef SYSTEM_UTILITIES_H
#define SYSTEM_UTILITIES_H

#include <stdint.h>

typedef struct
{
    uint32_t util_id;
    const char *util_name;
    const char *version;
} system_utilities_config_t;

int system_utilities_init(void);
int system_utilities_get_system_info(void);
int system_utilities_get_memory_usage(void);
int system_utilities_get_cpu_usage(void);
int system_utilities_get_disk_usage(const char *mount_point);
int system_utilities_restart_system(void);
int system_utilities_shutdown_system(void);

#endif
