#ifndef CODE_EDITOR_H
#define CODE_EDITOR_H

#include <stdint.h>

typedef struct
{
    uint32_t file_id;
    const char *file_path;
    const char *file_name;
    uint32_t line_count;
    uint32_t is_modified;
} editor_file_t;

typedef struct
{
    uint32_t total_files;
    uint32_t open_files;
    uint32_t modified_files;
    uint64_t total_lines;
} editor_stats_t;

int code_editor_init(void);
int code_editor_open_file(const char *file_path, uint32_t *file_id);
int code_editor_close_file(uint32_t file_id);
int code_editor_save_file(uint32_t file_id);
int code_editor_insert_text(uint32_t file_id, uint32_t line, uint32_t column, const char *text);
int code_editor_delete_text(uint32_t file_id, uint32_t line, uint32_t start_col, uint32_t end_col);
int code_editor_get_line(uint32_t file_id, uint32_t line, char *buffer, uint32_t max_len);
int code_editor_get_stats(editor_stats_t *stats);

#endif
