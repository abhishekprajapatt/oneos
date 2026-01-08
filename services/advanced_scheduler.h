#ifndef ONEOS_ADVANCED_SCHEDULER_H
#define ONEOS_ADVANCED_SCHEDULER_H

#include <stdint.h>
#include <time.h>

#define MAX_SCHEDULERS 16
#define MAX_SCHEDULING_DOMAINS 64
#define MAX_CPU_CORES 256
#define MAX_PROCESS_GROUPS 512
#define MAX_SCHEDULING_POLICIES 256
#define MAX_SCHEDULER_EVENTS 4096

typedef enum
{
    SCHEDULER_MODE_FIFO = 0,
    SCHEDULER_MODE_RR = 1,
    SCHEDULER_MODE_PRIORITY = 2,
    SCHEDULER_MODE_DEADLINE = 3,
    SCHEDULER_MODE_CFS = 4,
    SCHEDULER_MODE_NUMA_AWARE = 5
} SchedulerMode;

typedef enum
{
    PRIORITY_CLASS_IDLE = 0,
    PRIORITY_CLASS_BATCH = 1,
    PRIORITY_CLASS_NORMAL = 2,
    PRIORITY_CLASS_HIGH = 3,
    PRIORITY_CLASS_REALTIME = 4,
    PRIORITY_CLASS_DEADLINE = 5
} PriorityClass;

typedef enum
{
    CPU_AFFINITY_NONE = 0,
    CPU_AFFINITY_SINGLE = 1,
    CPU_AFFINITY_SOCKET = 2,
    CPU_AFFINITY_NUMA_NODE = 3,
    CPU_AFFINITY_CUSTOM = 4
} CPUAffinityMode;

typedef enum
{
    SCHEDULING_EVENT_PROCESS_CREATED = 0,
    SCHEDULING_EVENT_PROCESS_SCHEDULED = 1,
    SCHEDULING_EVENT_PROCESS_PREEMPTED = 2,
    SCHEDULING_EVENT_PROCESS_BLOCKED = 3,
    SCHEDULING_EVENT_PROCESS_WOKEN = 4,
    SCHEDULING_EVENT_CONTEXT_SWITCH = 5,
    SCHEDULING_EVENT_LOAD_BALANCED = 6,
    SCHEDULING_EVENT_AFFINITY_CHANGED = 7
} SchedulingEventType;

typedef struct
{
    uint32_t domain_id;
    char domain_name[64];
    uint32_t cpu_count;
    uint32_t cpus[MAX_CPU_CORES];
    uint32_t numa_node;
    uint64_t total_load;
    uint64_t available_load;
    uint8_t is_active;
} SchedulingDomain;

typedef struct
{
    uint32_t policy_id;
    char policy_name[64];
    SchedulerMode scheduler_mode;
    PriorityClass priority_class;
    uint32_t time_slice_ms;
    uint32_t priority_level;
    uint32_t deadline_ms;
    CPUAffinityMode affinity_mode;
    uint32_t cpu_mask;
    uint8_t inherit_priority;
    uint8_t preemptible;
    time_t created_at;
} SchedulingPolicy;

typedef struct
{
    uint32_t group_id;
    char group_name[64];
    uint32_t process_count;
    uint32_t parent_group_id;
    SchedulingPolicy policy;
    uint64_t cpu_time_used;
    uint64_t memory_usage;
    uint32_t context_switches;
    uint32_t page_faults;
    uint64_t io_operations;
    uint64_t io_bytes_transferred;
    double cpu_affinity_score;
    time_t created_at;
} ProcessGroup;

typedef struct
{
    uint64_t event_id;
    uint32_t process_id;
    SchedulingEventType event_type;
    uint32_t source_cpu;
    uint32_t target_cpu;
    uint64_t timestamp;
    uint64_t latency_us;
    char details[256];
} SchedulingEvent;

typedef struct
{
    uint32_t scheduler_id;
    char name[64];
    SchedulerMode current_mode;
    uint32_t domain_count;
    SchedulingDomain domains[MAX_SCHEDULING_DOMAINS];
    uint32_t policy_count;
    SchedulingPolicy policies[MAX_SCHEDULING_POLICIES];
    uint32_t process_group_count;
    ProcessGroup process_groups[MAX_PROCESS_GROUPS];
    uint32_t event_count;
    SchedulingEvent events[MAX_SCHEDULER_EVENTS];

    uint64_t total_context_switches;
    uint64_t total_preemptions;
    uint64_t total_load_balances;
    uint64_t total_affinity_changes;

    double average_latency_us;
    double average_cpu_utilization;
    uint8_t numa_aware_enabled;
    uint8_t priority_inheritance_enabled;

    time_t created_at;
    time_t last_rebalance;
    uint8_t is_active;
} AdvancedScheduler;

int as_create_scheduler(const char *name, SchedulerMode mode, uint32_t cpu_count);
int as_delete_scheduler(uint32_t scheduler_id);

int as_create_scheduling_domain(uint32_t scheduler_id, const char *domain_name,
                                uint32_t cpu_count, uint32_t *cpus, uint32_t numa_node);
int as_remove_scheduling_domain(uint32_t scheduler_id, uint32_t domain_id);

int as_create_scheduling_policy(uint32_t scheduler_id, const char *policy_name,
                                SchedulerMode mode, PriorityClass priority,
                                uint32_t time_slice_ms, uint8_t preemptible);
int as_apply_scheduling_policy(uint32_t scheduler_id, uint32_t process_id, uint32_t policy_id);
int as_delete_scheduling_policy(uint32_t scheduler_id, uint32_t policy_id);

int as_create_process_group(uint32_t scheduler_id, const char *group_name,
                            uint32_t parent_group_id);
int as_add_process_to_group(uint32_t scheduler_id, uint32_t group_id, uint32_t process_id);
int as_remove_process_from_group(uint32_t scheduler_id, uint32_t group_id, uint32_t process_id);
int as_delete_process_group(uint32_t scheduler_id, uint32_t group_id);

int as_set_cpu_affinity(uint32_t scheduler_id, uint32_t process_id, CPUAffinityMode mode,
                        uint32_t cpu_mask);
int as_get_cpu_affinity(uint32_t scheduler_id, uint32_t process_id, CPUAffinityMode *mode,
                        uint32_t *cpu_mask);

int as_set_priority(uint32_t scheduler_id, uint32_t process_id, uint32_t priority);
int as_enable_priority_inheritance(uint32_t scheduler_id);
int as_disable_priority_inheritance(uint32_t scheduler_id);

int as_perform_load_balancing(uint32_t scheduler_id);
int as_enable_numa_awareness(uint32_t scheduler_id);
int as_disable_numa_awareness(uint32_t scheduler_id);

int as_log_scheduling_event(uint32_t scheduler_id, uint32_t process_id,
                            SchedulingEventType event_type, uint32_t source_cpu,
                            uint32_t target_cpu, uint64_t latency_us, const char *details);
int as_get_scheduling_events(uint32_t scheduler_id, SchedulingEvent *events, uint32_t *count);

int as_get_process_group_stats(uint32_t scheduler_id, uint32_t group_id,
                               uint64_t *cpu_time, uint64_t *memory_usage,
                               uint32_t *context_switches);
int as_get_process_groups(uint32_t scheduler_id, ProcessGroup *groups, uint32_t *count);

uint64_t as_get_total_context_switches(uint32_t scheduler_id);
uint64_t as_get_total_preemptions(uint32_t scheduler_id);
uint64_t as_get_total_load_balances(uint32_t scheduler_id);
double as_get_average_latency(uint32_t scheduler_id);
double as_get_average_cpu_utilization(uint32_t scheduler_id);

int as_rebalance_load(uint32_t scheduler_id);
int as_collect_scheduler_metrics(uint32_t scheduler_id);

AdvancedScheduler *as_get_scheduler_info(uint32_t scheduler_id);

#endif
