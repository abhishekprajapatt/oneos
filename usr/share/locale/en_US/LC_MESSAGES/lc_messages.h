#ifndef LC_MESSAGES_H
#define LC_MESSAGES_H

#include <stdint.h>

typedef struct
{
    const char *message_id;
    const char *translation;
    const char *context;
} message_entry_t;

typedef struct
{
    uint32_t total_messages;
    uint32_t translated;
    uint32_t untranslated;
    const char *language;
    const char *encoding;
} message_stats_t;

int lc_messages_init(void);
int lc_messages_get_translation(const char *message_id, char *translation, uint32_t max_len);
int lc_messages_get_plural_form(const char *message_id, uint32_t count, char *translation, uint32_t max_len);
int lc_messages_list_messages(message_entry_t *messages, uint32_t *count);
int lc_messages_search_message(const char *keyword, message_entry_t *messages, uint32_t *count);
int lc_messages_get_stats(message_stats_t *stats);

#endif
