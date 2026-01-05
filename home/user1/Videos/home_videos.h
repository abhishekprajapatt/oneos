#ifndef HOME_VIDEOS_H
#define HOME_VIDEOS_H

#include <stdint.h>

typedef struct
{
    uint32_t video_id;
    const char *video_name;
    uint32_t duration;
    uint32_t width;
    uint32_t height;
    uint32_t bitrate;
} home_videos_file_t;

typedef struct
{
    uint32_t playlist_id;
    const char *playlist_name;
    uint32_t video_count;
    uint32_t total_duration;
} home_videos_playlist_t;

int home_videos_init(const char *videos_dir);
int home_videos_scan_library(void);
int home_videos_create_playlist(const char *playlist_name);
int home_videos_add_video_to_playlist(uint32_t playlist_id, uint32_t video_id);
int home_videos_remove_video_from_playlist(uint32_t playlist_id, uint32_t video_id);
int home_videos_play_video(uint32_t video_id);

#endif
