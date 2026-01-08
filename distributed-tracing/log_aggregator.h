#ifndef LOG_AGGREGATOR_H
#define LOG_AGGREGATOR_H

#include <stdint.h>
#include <stddef.h>

#define MAX_LOG_SOURCES 512
#define MAX_LOG_ENTRIES 16384
#define MAX_LOG_FILTERS 256
#define MAX_LOG_PARSERS 64

typedef enum
{
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_CRITICAL
} LogLevel;

typedef enum
{
    LOG_SOURCE_APPLICATION,
    LOG_SOURCE_SYSTEM,
    LOG_SOURCE_KERNEL,
    LOG_SOURCE_SECURITY,
    LOG_SOURCE_NETWORK,
    LOG_SOURCE_DATABASE
} LogSource;

typedef struct
{
    uint32_t entry_id;
    uint64_t timestamp_nanoseconds;

    LogLevel log_level;
    LogSource log_source;

    char service_name[128];
    char hostname[128];
    char log_message[1024];

    uint32_t process_id;
    uint32_t thread_id;

    uint8_t is_sampled;
    uint8_t is_indexed;
} LogEntry;

typedef struct
{
    uint32_t filter_id;
    char filter_name[128];

    LogLevel min_log_level;
    LogSource source_mask;

    char service_pattern[256];
    char message_pattern[512];

    uint8_t is_enabled;
    uint8_t is_regex_pattern;
} LogFilter;

typedef struct
{
    uint32_t parser_id;
    char parser_name[128];
    char log_format[512];

    uint32_t field_count;
    uint8_t is_custom_parser;
} LogParser;

typedef struct
{
    uint32_t source_id;
    char source_name[256];
    LogSource source_type;

    uint64_t log_count;
    uint64_t total_bytes_received;

    uint8_t is_active;
    uint8_t is_streaming;
} LogSource_Struct;

typedef struct
{
    uint32_t aggregator_id;

    LogEntry log_entries[MAX_LOG_ENTRIES];
    uint32_t log_entry_count;
    uint32_t next_entry_index;

    LogSource_Struct log_sources[MAX_LOG_SOURCES];
    uint32_t log_source_count;

    LogFilter log_filters[MAX_LOG_FILTERS];
    uint32_t log_filter_count;

    LogParser log_parsers[MAX_LOG_PARSERS];
    uint32_t log_parser_count;

    uint64_t total_logs_ingested;
    uint32_t active_sources;
    uint32_t logs_per_second;
    uint64_t total_bytes_stored;
} LogAggregator;

LogAggregator *log_aggregator_init(void);
int log_aggregator_destroy(LogAggregator *aggregator);

uint32_t log_source_register(LogAggregator *aggregator, const char *source_name,
                             LogSource source_type);

int log_source_activate(LogAggregator *aggregator, uint32_t source_id);
int log_source_deactivate(LogAggregator *aggregator, uint32_t source_id);

int log_source_start_streaming(LogAggregator *aggregator, uint32_t source_id);
int log_source_stop_streaming(LogAggregator *aggregator, uint32_t source_id);

uint32_t log_entry_add(LogAggregator *aggregator, LogLevel level, LogSource source,
                       const char *service_name, const char *hostname,
                       const char *log_message, uint32_t process_id, uint32_t thread_id);

int log_entry_mark_sampled(LogAggregator *aggregator, uint32_t entry_id);
int log_entry_mark_indexed(LogAggregator *aggregator, uint32_t entry_id);

uint32_t log_filter_create(LogAggregator *aggregator, const char *filter_name,
                           LogLevel min_level, LogSource source_mask);

int log_filter_set_service_pattern(LogAggregator *aggregator, uint32_t filter_id,
                                   const char *pattern);

int log_filter_set_message_pattern(LogAggregator *aggregator, uint32_t filter_id,
                                   const char *pattern);

int log_filter_enable_regex(LogAggregator *aggregator, uint32_t filter_id);

int log_filter_enable(LogAggregator *aggregator, uint32_t filter_id);

int log_filter_disable(LogAggregator *aggregator, uint32_t filter_id);

uint32_t log_filter_search(LogAggregator *aggregator, uint32_t filter_id,
                           uint32_t *matching_entries, uint32_t max_results);

uint32_t log_parser_register(LogAggregator *aggregator, const char *parser_name,
                             const char *log_format);

int log_parser_parse_entry(LogAggregator *aggregator, uint32_t parser_id,
                           const char *raw_log, LogEntry *parsed_entry);

int log_aggregator_rotate_logs(LogAggregator *aggregator);

uint64_t log_aggregator_get_total_logs(LogAggregator *aggregator);

int log_aggregator_export_logs(LogAggregator *aggregator, const char *filename,
                               LogLevel min_level);

int log_aggregator_clear_logs(LogAggregator *aggregator);

typedef struct
{
    uint32_t aggregator_id;
    uint32_t log_entry_count;
    uint32_t active_sources;
    uint32_t total_filters;
    uint32_t total_parsers;
    uint64_t total_logs_ingested;
    uint64_t total_bytes_stored;
    uint32_t logs_per_second;
    uint32_t error_log_count;
} LogAggregatorMetrics;

int log_aggregator_get_metrics(LogAggregator *aggregator, LogAggregatorMetrics *metrics);

#endif
