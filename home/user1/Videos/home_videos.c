#include "home_videos.h"

int home_videos_init(const char *videos_dir)
{
    if (!videos_dir)
    {
        return -1;
    }
    return 0;
}

int home_videos_scan_library(void)
{
    return 0;
}

int home_videos_create_playlist(const char *playlist_name)
{
    if (!playlist_name)
    {
        return -1;
    }
    return 0;
}

int home_videos_add_video_to_playlist(uint32_t playlist_id, uint32_t video_id)
{
    return 0;
}

int home_videos_remove_video_from_playlist(uint32_t playlist_id, uint32_t video_id)
{
    return 0;
}

int home_videos_play_video(uint32_t video_id)
{
    return 0;
}
