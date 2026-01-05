#ifndef INFO_H
#define INFO_H

#include <stdint.h>

typedef struct
{
    const char *info_name;
    const char *info_file;
    const char *section;
    uint32_t page_count;
    uint64_t size;
} info_entry_t;

typedef struct
{
    uint32_t total_info_files;
    uint32_t total_sections;
    uint64_t total_size;
} info_stats_t;

int info_init(void);
int info_list_pages(info_entry_t *pages, uint32_t *count);
int info_get_page(const char *info_name, info_entry_t *page);
int info_search_by_section(const char *section, info_entry_t *pages, uint32_t *count);
int info_get_page_content(const char *info_name, char *content, uint32_t max_size);
int info_get_stats(info_stats_t *stats);

#endif
