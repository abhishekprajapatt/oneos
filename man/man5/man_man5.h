#ifndef MAN_MAN5_H
#define MAN_MAN5_H

#include <stdint.h>

typedef struct
{
    uint32_t file_format_id;
    const char *file_name;
    const char *format_description;
    const char *file_location;
    uint32_t format_type;
} man_man5_entry_t;

typedef struct
{
    uint32_t total_file_formats;
    uint32_t documented_formats;
    uint32_t config_files;
} man_man5_state_t;

int man_man5_init(void);
int man_man5_add_format(man_man5_entry_t *entry);
int man_man5_remove_format(uint32_t format_id);
int man_man5_get_format(const char *file_name, man_man5_entry_t *entry);
int man_man5_get_config_files(man_man5_entry_t *results, uint32_t *count);

#endif
