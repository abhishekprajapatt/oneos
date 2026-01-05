#include "etc_udev.h"

int etc_udev_init(void)
{
    return 0;
}

int etc_udev_load_rules(const char *rules_dir)
{
    if (!rules_dir)
    {
        return -1;
    }
    return 0;
}

int etc_udev_add_rule(etc_udev_rule_t *rule)
{
    if (!rule)
    {
        return -1;
    }
    return 0;
}

int etc_udev_remove_rule(uint32_t rule_id)
{
    return 0;
}

int etc_udev_trigger_event(const char *subsystem, const char *action)
{
    if (!subsystem || !action)
    {
        return -1;
    }
    return 0;
}

int etc_udev_reload_rules(void)
{
    return 0;
}
