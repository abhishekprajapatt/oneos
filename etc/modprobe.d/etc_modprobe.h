#ifndef ETC_MODPROBE_H
#define ETC_MODPROBE_H

#include <stdint.h>

typedef struct
{
    uint32_t rule_id;
    const char *module_name;
    const char *blacklist_reason;
    uint32_t flags;
} etc_modprobe_rule_t;

typedef struct
{
    uint32_t rule_count;
    uint32_t install_count;
    uint32_t remove_count;
} etc_modprobe_config_t;

int etc_modprobe_init(void);
int etc_modprobe_load_config(const char *filename);
int etc_modprobe_add_rule(etc_modprobe_rule_t *rule);
int etc_modprobe_remove_rule(uint32_t rule_id);
int etc_modprobe_blacklist_module(const char *module_name);
int etc_modprobe_get_module_options(const char *module_name, char *options, uint32_t size);

#endif
