#ifndef SYSTEM_CLEANER_H
#define SYSTEM_CLEANER_H

#include <stdint.h>

typedef struct
{
    uint64_t total_size;
    uint32_t file_count;
    const char *cleanup_category;
} cleanup_item_t;

typedef struct
{
    uint64_t total_freed;
    uint32_t files_removed;
    uint32_t scans_performed;
} cleaner_stats_t;

int system_cleaner_init(void);
int system_cleaner_scan_temp_files(cleanup_item_t *items, uint32_t *count);
int system_cleaner_scan_cache(cleanup_item_t *items, uint32_t *count);
int system_cleaner_scan_logs(cleanup_item_t *items, uint32_t *count);
int system_cleaner_scan_duplicate_files(cleanup_item_t *items, uint32_t *count);
int system_cleaner_clean_temp_files(void);
int system_cleaner_clean_cache(void);
int system_cleaner_clean_logs(void);
int system_cleaner_get_stats(cleaner_stats_t *stats);

#endif
