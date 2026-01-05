#ifndef ETC_ONEOS_H
#define ETC_ONEOS_H

#include <stdint.h>

typedef struct
{
    uint32_t config_id;
    const char *key;
    const char *value;
    uint32_t type;
} etc_oneos_config_t;

typedef struct
{
    uint32_t version;
    uint32_t config_count;
    uint32_t flags;
} etc_oneos_state_t;

int etc_oneos_init(void);
int etc_oneos_load_config(const char *filename);
int etc_oneos_save_config(const char *filename);
int etc_oneos_get_config(const char *key, char *value, uint32_t size);
int etc_oneos_set_config(const char *key, const char *value);
int etc_oneos_reload(void);

#endif
