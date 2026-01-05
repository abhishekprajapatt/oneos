#include "media_driver.h"

int media_init(void)
{
    return 0;
}

int media_register_entity(media_entity_t *entity)
{
    if (!entity)
    {
        return -1;
    }
    return 0;
}

int media_create_link(media_link_t *link)
{
    if (!link)
    {
        return -1;
    }
    return 0;
}

int media_setup_link(media_link_t *link, uint32_t flags)
{
    if (!link)
    {
        return -1;
    }
    return 0;
}
