#include "media_player.h"

int media_player_init(void)
{
    return 0;
}

int media_player_open_file(const char *file_path)
{
    if (!file_path)
    {
        return -1;
    }
    return 0;
}

int media_player_play(void)
{
    return 0;
}

int media_player_pause(void)
{
    return 0;
}

int media_player_stop(void)
{
    return 0;
}

int media_player_seek(uint32_t position)
{
    return 0;
}

int media_player_next(void)
{
    return 0;
}

int media_player_previous(void)
{
    return 0;
}

int media_player_set_volume(uint32_t volume)
{
    if (volume > 100)
    {
        return -1;
    }
    return 0;
}

int media_player_get_stats(media_player_stats_t *stats)
{
    if (!stats)
    {
        return -1;
    }
    return 0;
}
