#ifndef MUSIC_PLAYER_H
#define MUSIC_PLAYER_H

#include <stdint.h>

typedef struct
{
    uint32_t song_id;
    const char *song_title;
    const char *artist_name;
    const char *album_name;
    uint32_t duration;
} song_info_t;

typedef struct
{
    uint32_t song_id;
    const char *playlist_name;
    uint32_t song_order;
} playlist_entry_t;

int music_player_init(void);
int music_player_play_song(uint32_t song_id);
int music_player_play_album(const char *album_name);
int music_player_play_playlist(const char *playlist_name);
int music_player_pause(void);
int music_player_resume(void);
int music_player_create_playlist(const char *playlist_name);
int music_player_add_song_to_playlist(uint32_t song_id, const char *playlist_name);
int music_player_search_songs(const char *query, song_info_t *songs, uint32_t *count);

#endif
