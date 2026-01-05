#include "home_music.h"

int home_music_init(const char *music_dir)
{
    if (!music_dir)
    {
        return -1;
    }
    return 0;
}

int home_music_scan_library(void)
{
    return 0;
}

int home_music_create_playlist(const char *playlist_name)
{
    if (!playlist_name)
    {
        return -1;
    }
    return 0;
}

int home_music_add_track_to_playlist(uint32_t playlist_id, uint32_t track_id)
{
    return 0;
}

int home_music_remove_track_from_playlist(uint32_t playlist_id, uint32_t track_id)
{
    return 0;
}

int home_music_play_track(uint32_t track_id)
{
    return 0;
}
