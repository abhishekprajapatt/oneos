#ifndef MAN_H
#define MAN_H

#include <stdint.h>

typedef struct
{
    const char *man_page_name;
    const char *man_file;
    uint32_t section;
    const char *description;
    uint64_t size;
} man_entry_t;

typedef struct
{
    uint32_t total_man_pages;
    uint32_t sections_available;
    uint64_t total_size;
} man_stats_t;

int man_init(void);
int man_list_pages(man_entry_t *pages, uint32_t *count);
int man_get_page(const char *page_name, man_entry_t *page);
int man_search_by_section(uint32_t section, man_entry_t *pages, uint32_t *count);
int man_get_page_content(const char *page_name, char *content, uint32_t max_size);
int man_search_keyword(const char *keyword, man_entry_t *pages, uint32_t *count);
int man_get_stats(man_stats_t *stats);

#endif
