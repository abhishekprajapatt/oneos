#ifndef MPV_H
#define MPV_H

#include <stdint.h>

typedef struct
{
    const char *file_path;
    uint32_t start_time;
    uint32_t duration;
    uint32_t fullscreen;
    float playback_speed;
} mpv_playback_options_t;

typedef struct
{
    uint64_t current_position;
    uint64_t total_duration;
    float playback_speed;
    uint32_t is_paused;
    uint32_t volume_level;
} mpv_playback_state_t;

int mpv_init(void);
int mpv_play_file(const char *file_path);
int mpv_play_with_options(mpv_playback_options_t *options);
int mpv_pause(void);
int mpv_resume(void);
int mpv_stop(void);
int mpv_get_playback_state(mpv_playback_state_t *state);

#endif
