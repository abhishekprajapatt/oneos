#include "media_management.h"

int media_management_init(void)
{
    return 0;
}

int media_management_register_device(media_management_device_t *device)
{
    if (!device)
    {
        return -1;
    }
    return 0;
}

int media_management_unregister_device(uint32_t media_id)
{
    return 0;
}

int media_management_format_device(uint32_t media_id)
{
    return 0;
}

int media_management_get_device_info(uint32_t media_id, media_management_device_t *info)
{
    if (!info)
    {
        return -1;
    }
    return 0;
}

int media_management_eject_device(uint32_t media_id)
{
    return 0;
}
