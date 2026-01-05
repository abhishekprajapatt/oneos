#include "mpv.h"

int mpv_init(void)
{
    return 0;
}

int mpv_play_file(const char *file_path)
{
    if (!file_path)
    {
        return -1;
    }
    return 0;
}

int mpv_play_with_options(mpv_playback_options_t *options)
{
    if (!options || !options->file_path)
    {
        return -1;
    }
    return 0;
}

int mpv_pause(void)
{
    return 0;
}

int mpv_resume(void)
{
    return 0;
}

int mpv_stop(void)
{
    return 0;
}

int mpv_get_playback_state(mpv_playback_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
