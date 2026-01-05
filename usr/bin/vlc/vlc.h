#ifndef VLC_H
#define VLC_H

#include <stdint.h>

typedef struct
{
    const char *media_file;
    const char *playlist_file;
    uint32_t fullscreen;
    float audio_sync;
    const char *subtitle_file;
} vlc_playback_options_t;

typedef struct
{
    uint64_t current_time;
    uint64_t total_duration;
    uint32_t volume;
    const char *media_codec;
    uint32_t frame_rate;
} vlc_media_state_t;

int vlc_init(void);
int vlc_open_file(const char *file_path);
int vlc_play_with_options(vlc_playback_options_t *options);
int vlc_pause(void);
int vlc_stop(void);
int vlc_set_volume(uint32_t volume);
int vlc_get_media_state(vlc_media_state_t *state);

#endif
