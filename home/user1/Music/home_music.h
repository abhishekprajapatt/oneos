#ifndef HOME_MUSIC_H
#define HOME_MUSIC_H

#include <stdint.h>

typedef struct
{
    uint32_t track_id;
    const char *track_name;
    const char *artist;
    uint32_t duration;
    uint32_t bitrate;
} home_music_track_t;

typedef struct
{
    uint32_t playlist_id;
    const char *playlist_name;
    uint32_t track_count;
    uint64_t total_size;
} home_music_playlist_t;

int home_music_init(const char *music_dir);
int home_music_scan_library(void);
int home_music_create_playlist(const char *playlist_name);
int home_music_add_track_to_playlist(uint32_t playlist_id, uint32_t track_id);
int home_music_remove_track_from_playlist(uint32_t playlist_id, uint32_t track_id);
int home_music_play_track(uint32_t track_id);

#endif
