#ifndef CORE_H
#define CORE_H

#include <stdint.h>

typedef struct
{
    uint32_t module_id;
    const char *module_name;
    const char *version;
} core_config_t;

int core_init(void);
int core_load_kernel_module(const char *module_name);
int core_unload_kernel_module(const char *module_name);
int core_get_kernel_version(char *version, uint32_t max_len);
int core_configure_system(const char *config_file);
int core_start_services(void);

#endif
