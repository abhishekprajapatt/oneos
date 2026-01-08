#include "log_aggregator.h"
#include <stdlib.h>
#include <string.h>

static LogAggregator *global_log_aggregator = NULL;
static uint32_t next_entry_id = 1;
static uint32_t next_filter_id = 1;
static uint32_t next_parser_id = 1;
static uint32_t next_source_id = 1;

LogAggregator *log_aggregator_init(void)
{
    LogAggregator *aggregator = (LogAggregator *)malloc(sizeof(LogAggregator));
    if (!aggregator)
        return NULL;

    memset(aggregator, 0, sizeof(LogAggregator));
    aggregator->aggregator_id = 1;
    aggregator->log_entry_count = 0;
    aggregator->next_entry_index = 0;
    aggregator->log_source_count = 0;
    aggregator->log_filter_count = 0;
    aggregator->log_parser_count = 0;
    aggregator->total_logs_ingested = 0;
    aggregator->active_sources = 0;
    aggregator->logs_per_second = 0;
    aggregator->total_bytes_stored = 0;

    global_log_aggregator = aggregator;
    return aggregator;
}

int log_aggregator_destroy(LogAggregator *aggregator)
{
    if (!aggregator)
        return -1;

    free(aggregator);
    global_log_aggregator = NULL;
    return 0;
}

uint32_t log_source_register(LogAggregator *aggregator, const char *source_name,
                             LogSource source_type)
{
    if (!aggregator || aggregator->log_source_count >= MAX_LOG_SOURCES || !source_name)
        return 0;

    LogSource_Struct *source = &aggregator->log_sources[aggregator->log_source_count];
    memset(source, 0, sizeof(LogSource_Struct));

    source->source_id = next_source_id++;
    strncpy(source->source_name, source_name, sizeof(source->source_name) - 1);
    source->source_type = source_type;

    source->log_count = 0;
    source->total_bytes_received = 0;

    source->is_active = 0;
    source->is_streaming = 0;

    aggregator->log_source_count++;
    return source->source_id;
}

int log_source_activate(LogAggregator *aggregator, uint32_t source_id)
{
    if (!aggregator || source_id == 0)
        return -1;

    for (uint32_t i = 0; i < aggregator->log_source_count; i++)
    {
        if (aggregator->log_sources[i].source_id == source_id)
        {
            aggregator->log_sources[i].is_active = 1;
            aggregator->active_sources++;
            return 0;
        }
    }

    return -1;
}

int log_source_deactivate(LogAggregator *aggregator, uint32_t source_id)
{
    if (!aggregator || source_id == 0)
        return -1;

    for (uint32_t i = 0; i < aggregator->log_source_count; i++)
    {
        if (aggregator->log_sources[i].source_id == source_id)
        {
            if (aggregator->log_sources[i].is_active)
            {
                aggregator->log_sources[i].is_active = 0;
                aggregator->active_sources--;
            }
            return 0;
        }
    }

    return -1;
}

int log_source_start_streaming(LogAggregator *aggregator, uint32_t source_id)
{
    if (!aggregator || source_id == 0)
        return -1;

    for (uint32_t i = 0; i < aggregator->log_source_count; i++)
    {
        if (aggregator->log_sources[i].source_id == source_id)
        {
            aggregator->log_sources[i].is_streaming = 1;
            return 0;
        }
    }

    return -1;
}

int log_source_stop_streaming(LogAggregator *aggregator, uint32_t source_id)
{
    if (!aggregator || source_id == 0)
        return -1;

    for (uint32_t i = 0; i < aggregator->log_source_count; i++)
    {
        if (aggregator->log_sources[i].source_id == source_id)
        {
            aggregator->log_sources[i].is_streaming = 0;
            return 0;
        }
    }

    return -1;
}

uint32_t log_entry_add(LogAggregator *aggregator, LogLevel level, LogSource source,
                       const char *service_name, const char *hostname,
                       const char *log_message, uint32_t process_id, uint32_t thread_id)
{
    if (!aggregator || aggregator->log_entry_count >= MAX_LOG_ENTRIES || !service_name ||
        !hostname || !log_message)
        return 0;

    LogEntry *entry = &aggregator->log_entries[aggregator->next_entry_index];
    memset(entry, 0, sizeof(LogEntry));

    entry->entry_id = next_entry_id++;
    entry->timestamp_nanoseconds = 0;

    entry->log_level = level;
    entry->log_source = source;

    strncpy(entry->service_name, service_name, sizeof(entry->service_name) - 1);
    strncpy(entry->hostname, hostname, sizeof(entry->hostname) - 1);
    strncpy(entry->log_message, log_message, sizeof(entry->log_message) - 1);

    entry->process_id = process_id;
    entry->thread_id = thread_id;

    entry->is_sampled = 0;
    entry->is_indexed = 0;

    aggregator->next_entry_index = (aggregator->next_entry_index + 1) % MAX_LOG_ENTRIES;

    if (aggregator->log_entry_count < MAX_LOG_ENTRIES)
    {
        aggregator->log_entry_count++;
    }

    aggregator->total_logs_ingested++;
    aggregator->total_bytes_stored += strlen(log_message);

    return entry->entry_id;
}

int log_entry_mark_sampled(LogAggregator *aggregator, uint32_t entry_id)
{
    if (!aggregator || entry_id == 0)
        return -1;

    for (uint32_t i = 0; i < aggregator->log_entry_count; i++)
    {
        if (aggregator->log_entries[i].entry_id == entry_id)
        {
            aggregator->log_entries[i].is_sampled = 1;
            return 0;
        }
    }

    return -1;
}

int log_entry_mark_indexed(LogAggregator *aggregator, uint32_t entry_id)
{
    if (!aggregator || entry_id == 0)
        return -1;

    for (uint32_t i = 0; i < aggregator->log_entry_count; i++)
    {
        if (aggregator->log_entries[i].entry_id == entry_id)
        {
            aggregator->log_entries[i].is_indexed = 1;
            return 0;
        }
    }

    return -1;
}

uint32_t log_filter_create(LogAggregator *aggregator, const char *filter_name,
                           LogLevel min_level, LogSource source_mask)
{
    if (!aggregator || aggregator->log_filter_count >= MAX_LOG_FILTERS || !filter_name)
        return 0;

    LogFilter *filter = &aggregator->log_filters[aggregator->log_filter_count];
    memset(filter, 0, sizeof(LogFilter));

    filter->filter_id = next_filter_id++;
    strncpy(filter->filter_name, filter_name, sizeof(filter->filter_name) - 1);

    filter->min_log_level = min_level;
    filter->source_mask = source_mask;

    filter->is_enabled = 1;
    filter->is_regex_pattern = 0;

    aggregator->log_filter_count++;
    return filter->filter_id;
}

int log_filter_set_service_pattern(LogAggregator *aggregator, uint32_t filter_id,
                                   const char *pattern)
{
    if (!aggregator || filter_id == 0 || !pattern)
        return -1;

    for (uint32_t i = 0; i < aggregator->log_filter_count; i++)
    {
        if (aggregator->log_filters[i].filter_id == filter_id)
        {
            strncpy(aggregator->log_filters[i].service_pattern, pattern,
                    sizeof(aggregator->log_filters[i].service_pattern) - 1);
            return 0;
        }
    }

    return -1;
}

int log_filter_set_message_pattern(LogAggregator *aggregator, uint32_t filter_id,
                                   const char *pattern)
{
    if (!aggregator || filter_id == 0 || !pattern)
        return -1;

    for (uint32_t i = 0; i < aggregator->log_filter_count; i++)
    {
        if (aggregator->log_filters[i].filter_id == filter_id)
        {
            strncpy(aggregator->log_filters[i].message_pattern, pattern,
                    sizeof(aggregator->log_filters[i].message_pattern) - 1);
            return 0;
        }
    }

    return -1;
}

int log_filter_enable_regex(LogAggregator *aggregator, uint32_t filter_id)
{
    if (!aggregator || filter_id == 0)
        return -1;

    for (uint32_t i = 0; i < aggregator->log_filter_count; i++)
    {
        if (aggregator->log_filters[i].filter_id == filter_id)
        {
            aggregator->log_filters[i].is_regex_pattern = 1;
            return 0;
        }
    }

    return -1;
}

int log_filter_enable(LogAggregator *aggregator, uint32_t filter_id)
{
    if (!aggregator || filter_id == 0)
        return -1;

    for (uint32_t i = 0; i < aggregator->log_filter_count; i++)
    {
        if (aggregator->log_filters[i].filter_id == filter_id)
        {
            aggregator->log_filters[i].is_enabled = 1;
            return 0;
        }
    }

    return -1;
}

int log_filter_disable(LogAggregator *aggregator, uint32_t filter_id)
{
    if (!aggregator || filter_id == 0)
        return -1;

    for (uint32_t i = 0; i < aggregator->log_filter_count; i++)
    {
        if (aggregator->log_filters[i].filter_id == filter_id)
        {
            aggregator->log_filters[i].is_enabled = 0;
            return 0;
        }
    }

    return -1;
}

uint32_t log_filter_search(LogAggregator *aggregator, uint32_t filter_id,
                           uint32_t *matching_entries, uint32_t max_results)
{
    if (!aggregator || filter_id == 0 || !matching_entries)
        return 0;

    uint32_t count = 0;

    for (uint32_t i = 0; i < aggregator->log_filter_count; i++)
    {
        if (aggregator->log_filters[i].filter_id == filter_id && aggregator->log_filters[i].is_enabled)
        {
            LogFilter *filter = &aggregator->log_filters[i];

            for (uint32_t j = 0; j < aggregator->log_entry_count && count < max_results; j++)
            {
                LogEntry *entry = &aggregator->log_entries[j];

                if (entry->log_level >= filter->min_log_level &&
                    (filter->source_mask & (1 << entry->log_source)))
                {
                    matching_entries[count++] = entry->entry_id;
                }
            }

            break;
        }
    }

    return count;
}

uint32_t log_parser_register(LogAggregator *aggregator, const char *parser_name,
                             const char *log_format)
{
    if (!aggregator || aggregator->log_parser_count >= MAX_LOG_PARSERS || !parser_name || !log_format)
        return 0;

    LogParser *parser = &aggregator->log_parsers[aggregator->log_parser_count];
    memset(parser, 0, sizeof(LogParser));

    parser->parser_id = next_parser_id++;
    strncpy(parser->parser_name, parser_name, sizeof(parser->parser_name) - 1);
    strncpy(parser->log_format, log_format, sizeof(parser->log_format) - 1);

    parser->field_count = 0;
    parser->is_custom_parser = 0;

    aggregator->log_parser_count++;
    return parser->parser_id;
}

int log_parser_parse_entry(LogAggregator *aggregator, uint32_t parser_id,
                           const char *raw_log, LogEntry *parsed_entry)
{
    if (!aggregator || parser_id == 0 || !raw_log || !parsed_entry)
        return -1;

    for (uint32_t i = 0; i < aggregator->log_parser_count; i++)
    {
        if (aggregator->log_parsers[i].parser_id == parser_id)
        {
            memset(parsed_entry, 0, sizeof(LogEntry));
            parsed_entry->log_level = LOG_LEVEL_INFO;
            parsed_entry->log_source = LOG_SOURCE_APPLICATION;
            strncpy(parsed_entry->log_message, raw_log, sizeof(parsed_entry->log_message) - 1);
            return 0;
        }
    }

    return -1;
}

int log_aggregator_rotate_logs(LogAggregator *aggregator)
{
    if (!aggregator)
        return -1;

    aggregator->log_entry_count = 0;
    aggregator->next_entry_index = 0;
    memset(aggregator->log_entries, 0, sizeof(aggregator->log_entries));

    return 0;
}

uint64_t log_aggregator_get_total_logs(LogAggregator *aggregator)
{
    if (!aggregator)
        return 0;

    return aggregator->total_logs_ingested;
}

int log_aggregator_export_logs(LogAggregator *aggregator, const char *filename,
                               LogLevel min_level)
{
    if (!aggregator || !filename)
        return -1;

    uint32_t export_count = 0;

    for (uint32_t i = 0; i < aggregator->log_entry_count; i++)
    {
        if (aggregator->log_entries[i].log_level >= min_level)
        {
            export_count++;
        }
    }

    return export_count;
}

int log_aggregator_clear_logs(LogAggregator *aggregator)
{
    if (!aggregator)
        return -1;

    aggregator->log_entry_count = 0;
    aggregator->next_entry_index = 0;
    memset(aggregator->log_entries, 0, sizeof(aggregator->log_entries));

    return 0;
}

int log_aggregator_get_metrics(LogAggregator *aggregator, LogAggregatorMetrics *metrics)
{
    if (!aggregator || !metrics)
        return -1;

    metrics->aggregator_id = aggregator->aggregator_id;
    metrics->log_entry_count = aggregator->log_entry_count;
    metrics->active_sources = aggregator->active_sources;
    metrics->total_filters = aggregator->log_filter_count;
    metrics->total_parsers = aggregator->log_parser_count;
    metrics->total_logs_ingested = aggregator->total_logs_ingested;
    metrics->total_bytes_stored = aggregator->total_bytes_stored;
    metrics->logs_per_second = aggregator->logs_per_second;

    uint32_t error_count = 0;
    for (uint32_t i = 0; i < aggregator->log_entry_count; i++)
    {
        if (aggregator->log_entries[i].log_level >= LOG_LEVEL_ERROR)
        {
            error_count++;
        }
    }

    metrics->error_log_count = error_count;

    return 0;
}
