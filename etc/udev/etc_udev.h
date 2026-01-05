#ifndef ETC_UDEV_H
#define ETC_UDEV_H

#include <stdint.h>

typedef struct
{
    uint32_t rule_id;
    const char *subsystem;
    const char *action;
    const char *attribute;
    const char *value;
} etc_udev_rule_t;

typedef struct
{
    uint32_t rule_count;
    uint32_t device_count;
    uint32_t event_count;
} etc_udev_state_t;

int etc_udev_init(void);
int etc_udev_load_rules(const char *rules_dir);
int etc_udev_add_rule(etc_udev_rule_t *rule);
int etc_udev_remove_rule(uint32_t rule_id);
int etc_udev_trigger_event(const char *subsystem, const char *action);
int etc_udev_reload_rules(void);

#endif
