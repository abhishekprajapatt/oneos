#include "media.h"

int media_init(void)
{
    return 0;
}

int media_load(const char *filename, media_info_t *info)
{
    if (!filename || !info)
    {
        return -1;
    }
    info->width = 0;
    info->height = 0;
    info->duration = 0;
    return 0;
}

int media_play(void)
{
    return 0;
}

int media_pause(void)
{
    return 0;
}

int media_stop(void)
{
    return 0;
}

int media_seek(uint32_t position)
{
    return 0;
}
