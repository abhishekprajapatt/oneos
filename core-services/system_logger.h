#ifndef SYSTEM_LOGGER_H
#define SYSTEM_LOGGER_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#define MAX_LOG_ENTRIES 10000
#define MAX_LOG_MESSAGE_LEN 512
#define MAX_LOG_FILES 256
#define MAX_LOG_BUFFER_SIZE 65536

typedef enum
{
    LOG_LEVEL_DEBUG = 0,
    LOG_LEVEL_INFO = 1,
    LOG_LEVEL_WARNING = 2,
    LOG_LEVEL_ERROR = 3,
    LOG_LEVEL_CRITICAL = 4
} LogLevel;

typedef enum
{
    LOG_TARGET_MEMORY = 1,
    LOG_TARGET_FILE = 2,
    LOG_TARGET_SERIAL = 4,
    LOG_TARGET_NETWORK = 8
} LogTarget;

typedef struct
{
    uint32_t log_id;
    time_t timestamp;
    LogLevel level;
    uint32_t source_pid;
    uint32_t source_tid;
    char module[64];
    char message[MAX_LOG_MESSAGE_LEN];
} LogEntry;

typedef struct
{
    LogEntry entries[MAX_LOG_ENTRIES];
    uint32_t entry_count;
    uint32_t oldest_entry_index;
    LogLevel filter_level;
    LogTarget targets;
    uint64_t total_logs;
    uint64_t debug_count;
    uint64_t info_count;
    uint64_t warning_count;
    uint64_t error_count;
    uint64_t critical_count;
} SystemLogger;

typedef struct
{
    uint32_t pid;
    uint64_t total_syscalls;
    uint64_t total_context_switches;
    uint64_t memory_used;
    uint32_t thread_count;
    uint32_t open_file_descriptors;
} ProcessInfo;

typedef struct
{
    uint32_t cpu_count;
    uint64_t total_memory;
    uint64_t used_memory;
    uint64_t free_memory;
    uint32_t running_processes;
    uint32_t total_processes;
    double cpu_usage_percent;
    uint64_t page_faults;
    uint64_t disk_reads;
    uint64_t disk_writes;
} SystemMonitor;

typedef struct
{
    SystemLogger logger;
    SystemMonitor monitor;
    ProcessInfo processes[256];
    uint32_t process_count;
    time_t last_update;
} SystemLoggingManager;

SystemLoggingManager *logger_init(void);
void logger_cleanup(SystemLoggingManager *manager);

int logger_log(SystemLoggingManager *manager, LogLevel level, uint32_t pid, uint32_t tid,
               const char *module, const char *message);
int logger_set_filter_level(SystemLoggingManager *manager, LogLevel level);
int logger_set_targets(SystemLoggingManager *manager, LogTarget targets);
LogLevel logger_get_filter_level(SystemLoggingManager *manager);

LogEntry *logger_get_entry(SystemLoggingManager *manager, uint32_t index);
uint32_t logger_get_entry_count(SystemLoggingManager *manager);
uint64_t logger_get_total_logs(SystemLoggingManager *manager);
uint64_t logger_get_log_count_by_level(SystemLoggingManager *manager, LogLevel level);

int logger_clear_entries(SystemLoggingManager *manager);
int logger_export_logs(SystemLoggingManager *manager, const char *filename);

int monitor_update(SystemLoggingManager *manager);
int monitor_register_process(SystemLoggingManager *manager, uint32_t pid);
int monitor_update_process_info(SystemLoggingManager *manager, uint32_t pid, ProcessInfo *info);

SystemMonitor *monitor_get_info(SystemLoggingManager *manager);
ProcessInfo *monitor_get_process_info(SystemLoggingManager *manager, uint32_t pid);
uint32_t monitor_get_process_count(SystemLoggingManager *manager);

double monitor_get_cpu_usage(SystemLoggingManager *manager);
uint64_t monitor_get_memory_usage(SystemLoggingManager *manager);
uint32_t monitor_get_running_processes(SystemLoggingManager *manager);

#endif
