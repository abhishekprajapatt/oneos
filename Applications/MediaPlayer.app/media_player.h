#ifndef MEDIA_PLAYER_H
#define MEDIA_PLAYER_H

#include <stdint.h>

typedef struct
{
    uint32_t media_id;
    const char *media_title;
    const char *media_artist;
    const char *media_format;
    uint32_t duration;
} media_info_t;

typedef struct
{
    uint32_t total_media;
    uint32_t total_playlists;
    uint64_t total_duration;
} media_player_stats_t;

int media_player_init(void);
int media_player_open_file(const char *file_path);
int media_player_play(void);
int media_player_pause(void);
int media_player_stop(void);
int media_player_seek(uint32_t position);
int media_player_next(void);
int media_player_previous(void);
int media_player_set_volume(uint32_t volume);
int media_player_get_stats(media_player_stats_t *stats);

#endif
