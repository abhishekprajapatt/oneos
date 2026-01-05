#ifndef ONEOS_H
#define ONEOS_H

#include <stdint.h>

typedef struct
{
    uint32_t os_id;
    const char *os_name;
    const char *os_version;
    uint32_t build_number;
} oneos_config_t;

int oneos_init(void);
int oneos_start(void);
int oneos_shutdown(void);
int oneos_get_version(char *version, uint32_t max_len);
int oneos_get_system_info(void);
int oneos_load_configuration(const char *config_file);

#endif
