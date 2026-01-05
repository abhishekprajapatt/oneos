#ifndef NOTES_H
#define NOTES_H

#include <stdint.h>

typedef struct
{
    uint32_t note_id;
    const char *note_title;
    const char *note_content;
    uint32_t created_time;
    uint32_t modified_time;
    const char *category;
} note_info_t;

typedef struct
{
    uint32_t total_notes;
    uint32_t total_categories;
    uint64_t total_size;
} notes_stats_t;

int notes_init(void);
int notes_create(note_info_t *note);
int notes_delete(uint32_t note_id);
int notes_update(note_info_t *note);
int notes_get(uint32_t note_id, note_info_t *note);
int notes_list_all(note_info_t *notes, uint32_t *count);
int notes_search(const char *query, note_info_t *results, uint32_t *count);
int notes_get_stats(notes_stats_t *stats);

#endif
