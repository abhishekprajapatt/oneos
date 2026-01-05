#ifndef WORD_PROCESSOR_H
#define WORD_PROCESSOR_H

#include <stdint.h>

typedef struct
{
    uint32_t doc_id;
    const char *doc_title;
    const char *doc_path;
    uint32_t word_count;
    uint32_t page_count;
    uint32_t is_modified;
} document_info_t;

typedef struct
{
    uint32_t total_documents;
    uint32_t open_documents;
    uint32_t unsaved_documents;
} word_processor_stats_t;

int word_processor_init(void);
int word_processor_create_document(const char *title, uint32_t *doc_id);
int word_processor_open_document(const char *file_path, uint32_t *doc_id);
int word_processor_close_document(uint32_t doc_id);
int word_processor_save_document(uint32_t doc_id);
int word_processor_insert_text(uint32_t doc_id, const char *text);
int word_processor_format_text(uint32_t doc_id, const char *format_type);
int word_processor_export_document(uint32_t doc_id, const char *format);
int word_processor_get_stats(word_processor_stats_t *stats);

#endif
