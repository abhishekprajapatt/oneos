#include "init_boot.h"

int init_boot_load_config(const char *config_file)
{
    if (!config_file)
    {
        return -1;
    }
    return 0;
}

int init_boot_add_sequence(init_boot_sequence_t *sequence)
{
    if (!sequence)
    {
        return -1;
    }
    return 0;
}

int init_boot_remove_sequence(uint32_t boot_id)
{
    return 0;
}

int init_boot_execute_sequence(uint32_t boot_id)
{
    return 0;
}

int init_boot_get_status(uint32_t *status)
{
    if (!status)
    {
        return -1;
    }
    return 0;
}

int init_boot_wait_complete(uint32_t timeout)
{
    return 0;
}
