#include "system_logger.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

SystemLoggingManager *logger_init(void)
{
    SystemLoggingManager *manager = (SystemLoggingManager *)malloc(sizeof(SystemLoggingManager));
    if (!manager)
        return NULL;

    memset(&manager->logger, 0, sizeof(SystemLogger));
    memset(&manager->monitor, 0, sizeof(SystemMonitor));
    memset(manager->processes, 0, 256 * sizeof(ProcessInfo));

    manager->logger.filter_level = LOG_LEVEL_DEBUG;
    manager->logger.targets = LOG_TARGET_MEMORY;
    manager->process_count = 0;
    manager->last_update = time(NULL);

    manager->monitor.cpu_count = 4;
    manager->monitor.total_memory = 8589934592ULL;
    manager->monitor.free_memory = manager->monitor.total_memory;
    manager->monitor.used_memory = 0;

    return manager;
}

void logger_cleanup(SystemLoggingManager *manager)
{
    if (!manager)
        return;
    free(manager);
}

int logger_log(SystemLoggingManager *manager, LogLevel level, uint32_t pid, uint32_t tid,
               const char *module, const char *message)
{
    if (!manager || !module || !message)
        return -1;

    if (level < manager->logger.filter_level)
        return 0;

    if (manager->logger.entry_count >= MAX_LOG_ENTRIES)
    {
        manager->logger.oldest_entry_index = (manager->logger.oldest_entry_index + 1) % MAX_LOG_ENTRIES;
    }

    uint32_t index = manager->logger.entry_count % MAX_LOG_ENTRIES;
    LogEntry *entry = &manager->logger.entries[index];

    entry->log_id = manager->logger.total_logs + 1;
    entry->timestamp = time(NULL);
    entry->level = level;
    entry->source_pid = pid;
    entry->source_tid = tid;

    strncpy(entry->module, module, 63);
    entry->module[63] = '\0';

    strncpy(entry->message, message, MAX_LOG_MESSAGE_LEN - 1);
    entry->message[MAX_LOG_MESSAGE_LEN - 1] = '\0';

    manager->logger.total_logs++;

    switch (level)
    {
    case LOG_LEVEL_DEBUG:
        manager->logger.debug_count++;
        break;
    case LOG_LEVEL_INFO:
        manager->logger.info_count++;
        break;
    case LOG_LEVEL_WARNING:
        manager->logger.warning_count++;
        break;
    case LOG_LEVEL_ERROR:
        manager->logger.error_count++;
        break;
    case LOG_LEVEL_CRITICAL:
        manager->logger.critical_count++;
        break;
    }

    if (manager->logger.entry_count < MAX_LOG_ENTRIES)
    {
        manager->logger.entry_count++;
    }

    return 0;
}

int logger_set_filter_level(SystemLoggingManager *manager, LogLevel level)
{
    if (!manager || level > LOG_LEVEL_CRITICAL)
        return -1;
    manager->logger.filter_level = level;
    return 0;
}

int logger_set_targets(SystemLoggingManager *manager, LogTarget targets)
{
    if (!manager)
        return -1;
    manager->logger.targets = targets;
    return 0;
}

LogLevel logger_get_filter_level(SystemLoggingManager *manager)
{
    if (!manager)
        return LOG_LEVEL_DEBUG;
    return manager->logger.filter_level;
}

LogEntry *logger_get_entry(SystemLoggingManager *manager, uint32_t index)
{
    if (!manager || index >= manager->logger.entry_count)
        return NULL;
    return &manager->logger.entries[index];
}

uint32_t logger_get_entry_count(SystemLoggingManager *manager)
{
    if (!manager)
        return 0;
    return manager->logger.entry_count;
}

uint64_t logger_get_total_logs(SystemLoggingManager *manager)
{
    if (!manager)
        return 0;
    return manager->logger.total_logs;
}

uint64_t logger_get_log_count_by_level(SystemLoggingManager *manager, LogLevel level)
{
    if (!manager)
        return 0;

    switch (level)
    {
    case LOG_LEVEL_DEBUG:
        return manager->logger.debug_count;
    case LOG_LEVEL_INFO:
        return manager->logger.info_count;
    case LOG_LEVEL_WARNING:
        return manager->logger.warning_count;
    case LOG_LEVEL_ERROR:
        return manager->logger.error_count;
    case LOG_LEVEL_CRITICAL:
        return manager->logger.critical_count;
    }

    return 0;
}

int logger_clear_entries(SystemLoggingManager *manager)
{
    if (!manager)
        return -1;

    memset(manager->logger.entries, 0, MAX_LOG_ENTRIES * sizeof(LogEntry));
    manager->logger.entry_count = 0;
    manager->logger.oldest_entry_index = 0;

    return 0;
}

int logger_export_logs(SystemLoggingManager *manager, const char *filename)
{
    if (!manager || !filename)
        return -1;

    FILE *file = fopen(filename, "w");
    if (!file)
        return -1;

    fprintf(file, "=== OneOS System Logs ===\n");
    fprintf(file, "Total Logs: %llu\n", manager->logger.total_logs);
    fprintf(file, "Debug: %llu, Info: %llu, Warning: %llu, Error: %llu, Critical: %llu\n\n",
            manager->logger.debug_count, manager->logger.info_count,
            manager->logger.warning_count, manager->logger.error_count,
            manager->logger.critical_count);

    for (uint32_t i = 0; i < manager->logger.entry_count; i++)
    {
        LogEntry *entry = &manager->logger.entries[i];
        const char *level_str = "DEBUG";

        if (entry->level == LOG_LEVEL_INFO)
            level_str = "INFO";
        else if (entry->level == LOG_LEVEL_WARNING)
            level_str = "WARN";
        else if (entry->level == LOG_LEVEL_ERROR)
            level_str = "ERROR";
        else if (entry->level == LOG_LEVEL_CRITICAL)
            level_str = "CRITICAL";

        fprintf(file, "[%s] [PID:%u] [%s] %s: %s\n", level_str, entry->source_pid,
                entry->module, ctime(&entry->timestamp), entry->message);
    }

    fclose(file);
    return 0;
}

int monitor_update(SystemLoggingManager *manager)
{
    if (!manager)
        return -1;

    manager->monitor.used_memory = (manager->monitor.total_memory * 60) / 100;
    manager->monitor.free_memory = manager->monitor.total_memory - manager->monitor.used_memory;
    manager->monitor.running_processes = manager->process_count;
    manager->monitor.cpu_usage_percent = 25.5;
    manager->last_update = time(NULL);

    return 0;
}

int monitor_register_process(SystemLoggingManager *manager, uint32_t pid)
{
    if (!manager || manager->process_count >= 256)
        return -1;

    ProcessInfo *proc = &manager->processes[manager->process_count];
    proc->pid = pid;
    proc->total_syscalls = 0;
    proc->total_context_switches = 0;
    proc->memory_used = 0;
    proc->thread_count = 1;
    proc->open_file_descriptors = 3;

    manager->process_count++;
    manager->monitor.running_processes = manager->process_count;

    return 0;
}

int monitor_update_process_info(SystemLoggingManager *manager, uint32_t pid, ProcessInfo *info)
{
    if (!manager || !info)
        return -1;

    for (uint32_t i = 0; i < manager->process_count; i++)
    {
        if (manager->processes[i].pid == pid)
        {
            memcpy(&manager->processes[i], info, sizeof(ProcessInfo));
            return 0;
        }
    }

    return -1;
}

SystemMonitor *monitor_get_info(SystemLoggingManager *manager)
{
    if (!manager)
        return NULL;
    return &manager->monitor;
}

ProcessInfo *monitor_get_process_info(SystemLoggingManager *manager, uint32_t pid)
{
    if (!manager)
        return NULL;

    for (uint32_t i = 0; i < manager->process_count; i++)
    {
        if (manager->processes[i].pid == pid)
        {
            return &manager->processes[i];
        }
    }

    return NULL;
}

uint32_t monitor_get_process_count(SystemLoggingManager *manager)
{
    if (!manager)
        return 0;
    return manager->process_count;
}

double monitor_get_cpu_usage(SystemLoggingManager *manager)
{
    if (!manager)
        return 0.0;
    return manager->monitor.cpu_usage_percent;
}

uint64_t monitor_get_memory_usage(SystemLoggingManager *manager)
{
    if (!manager)
        return 0;
    return manager->monitor.used_memory;
}

uint32_t monitor_get_running_processes(SystemLoggingManager *manager)
{
    if (!manager)
        return 0;
    return manager->monitor.running_processes;
}
