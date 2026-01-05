#ifndef FINDER_H
#define FINDER_H

#include <stdint.h>

typedef struct
{
    uint32_t result_id;
    const char *result_name;
    const char *result_path;
    const char *result_type;
    uint64_t result_size;
} search_result_t;

typedef struct
{
    uint32_t total_results;
    uint32_t displayed_results;
    uint32_t search_time_ms;
} finder_stats_t;

int finder_init(void);
int finder_search(const char *query, search_result_t *results, uint32_t *count);
int finder_search_by_type(const char *file_type, search_result_t *results, uint32_t *count);
int finder_search_by_date(uint32_t start_date, uint32_t end_date, search_result_t *results, uint32_t *count);
int finder_search_by_size(uint64_t min_size, uint64_t max_size, search_result_t *results, uint32_t *count);
int finder_open_file(const char *file_path);
int finder_preview_file(const char *file_path);
int finder_get_stats(finder_stats_t *stats);

#endif
