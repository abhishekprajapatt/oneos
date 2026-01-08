#ifndef ONEOS_WORKLOAD_MANAGER_H
#define ONEOS_WORKLOAD_MANAGER_H

#include <stdint.h>
#include <time.h>

#define MAX_WORKLOAD_MANAGERS 16
#define MAX_WORKLOAD_TYPES 64
#define MAX_JOBS 2048
#define MAX_JOB_WORKFLOWS 256
#define MAX_BATCH_TASKS 4096
#define MAX_WORKLOAD_TEMPLATES 512

typedef enum
{
    WORKLOAD_TYPE_BATCH = 0,
    WORKLOAD_TYPE_STREAMING = 1,
    WORKLOAD_TYPE_INTERACTIVE = 2,
    WORKLOAD_TYPE_BACKGROUND = 3,
    WORKLOAD_TYPE_DEADLINE_CRITICAL = 4,
    WORKLOAD_TYPE_BEST_EFFORT = 5
} WorkloadType;

typedef enum
{
    JOB_STATE_CREATED = 0,
    JOB_STATE_QUEUED = 1,
    JOB_STATE_SCHEDULED = 2,
    JOB_STATE_RUNNING = 3,
    JOB_STATE_SUSPENDED = 4,
    JOB_STATE_COMPLETED = 5,
    JOB_STATE_FAILED = 6,
    JOB_STATE_CANCELED = 7
} JobState;

typedef enum
{
    TASK_STATE_PENDING = 0,
    TASK_STATE_RUNNING = 1,
    TASK_STATE_COMPLETED = 2,
    TASK_STATE_FAILED = 3,
    TASK_STATE_RETRYING = 4,
    TASK_STATE_SKIPPED = 5
} TaskState;

typedef enum
{
    SCHEDULING_PRIORITY_LOW = 0,
    SCHEDULING_PRIORITY_MEDIUM = 1,
    SCHEDULING_PRIORITY_HIGH = 2,
    SCHEDULING_PRIORITY_CRITICAL = 3
} SchedulingPriority;

typedef enum
{
    DEPENDENCY_TYPE_FINISH_START = 0,
    DEPENDENCY_TYPE_START_START = 1,
    DEPENDENCY_TYPE_FINISH_FINISH = 2,
    DEPENDENCY_TYPE_START_FINISH = 3
} DependencyType;

typedef struct
{
    uint32_t workflow_id;
    char workflow_name[64];
    uint32_t stage;
    uint32_t total_stages;
    uint64_t job_count;
    uint8_t is_parallel;
    time_t created_at;
} JobWorkflow;

typedef struct
{
    uint64_t task_id;
    uint64_t job_id;
    char task_name[64];
    TaskState state;
    uint32_t retry_count;
    uint32_t max_retries;
    time_t started_at;
    time_t completed_at;
    uint32_t duration_seconds;
    uint8_t success;
    char error_message[256];
} BatchTask;

typedef struct
{
    uint64_t job_id;
    char job_name[64];
    WorkloadType workload_type;
    JobState current_state;
    SchedulingPriority priority;
    uint32_t workflow_id;
    uint32_t task_count;
    uint32_t completed_tasks;
    uint32_t failed_tasks;
    double progress_percentage;

    time_t submitted_at;
    time_t started_at;
    time_t completed_at;
    uint32_t total_duration_seconds;
    uint32_t estimated_remaining_seconds;

    uint32_t owner_id;
    uint64_t required_memory_mb;
    uint32_t required_cpu_cores;
    uint32_t gpu_count;

    double estimated_cost;
    double actual_cost;

    char result_status[32];
    uint32_t exit_code;
} Job;

typedef struct
{
    uint32_t template_id;
    char template_name[64];
    WorkloadType workload_type;
    char description[256];
    uint32_t default_priority;
    uint64_t default_memory_mb;
    uint32_t default_cpu_cores;
    uint32_t default_gpu_count;
    uint32_t estimated_duration_seconds;
    uint8_t is_template;
} WorkloadTemplate;

typedef struct
{
    uint32_t manager_id;
    char name[64];

    uint32_t workload_type_count;
    uint32_t active_workload_types[MAX_WORKLOAD_TYPES];

    uint32_t job_count;
    Job jobs[MAX_JOBS];

    uint32_t workflow_count;
    JobWorkflow workflows[MAX_JOB_WORKFLOWS];

    uint32_t batch_task_count;
    BatchTask batch_tasks[MAX_BATCH_TASKS];

    uint32_t template_count;
    WorkloadTemplate templates[MAX_WORKLOAD_TEMPLATES];

    uint64_t total_jobs_submitted;
    uint64_t total_jobs_completed;
    uint64_t total_jobs_failed;

    uint64_t total_tasks_executed;
    uint64_t total_tasks_completed;
    uint64_t total_tasks_failed;

    double average_job_duration_seconds;
    double average_task_duration_seconds;
    double job_success_rate;

    uint64_t total_workload_cost;

    time_t created_at;
    time_t last_job_submitted;
    uint8_t is_active;
} WorkloadManager;

int wm_create_manager(const char *name);
int wm_delete_manager(uint32_t manager_id);

int wm_submit_job(uint32_t manager_id, const char *job_name, WorkloadType workload_type,
                  SchedulingPriority priority, uint32_t owner_id, uint64_t memory_mb,
                  uint32_t cpu_cores, uint32_t gpu_count);
int wm_cancel_job(uint32_t manager_id, uint64_t job_id);
int wm_suspend_job(uint32_t manager_id, uint64_t job_id);
int wm_resume_job(uint32_t manager_id, uint64_t job_id);
int wm_get_job_status(uint32_t manager_id, uint64_t job_id, JobState *state,
                      double *progress);
int wm_get_jobs(uint32_t manager_id, Job *jobs, uint32_t *count);

int wm_create_job_workflow(uint32_t manager_id, const char *workflow_name,
                           uint32_t total_stages, uint8_t is_parallel);
int wm_add_job_to_workflow(uint32_t manager_id, uint32_t workflow_id, uint64_t job_id,
                           uint32_t stage);
int wm_get_workflows(uint32_t manager_id, JobWorkflow *workflows, uint32_t *count);

int wm_submit_batch_task(uint32_t manager_id, uint64_t job_id, const char *task_name,
                         uint32_t max_retries);
int wm_complete_batch_task(uint32_t manager_id, uint64_t task_id, uint8_t success,
                           const char *error_msg);
int wm_get_batch_tasks(uint32_t manager_id, BatchTask *tasks, uint32_t *count);

int wm_create_workload_template(uint32_t manager_id, const char *template_name,
                                WorkloadType workload_type, const char *description,
                                uint64_t memory_mb, uint32_t cpu_cores);
int wm_get_workload_templates(uint32_t manager_id, WorkloadTemplate *templates,
                              uint32_t *count);
int wm_create_job_from_template(uint32_t manager_id, uint32_t template_id,
                                const char *job_name, uint32_t owner_id);

int wm_set_job_priority(uint32_t manager_id, uint64_t job_id, SchedulingPriority priority);
int wm_estimate_job_cost(uint32_t manager_id, uint64_t job_id, double *estimated_cost);
int wm_get_job_cost(uint32_t manager_id, uint64_t job_id, double *actual_cost);

int wm_schedule_job(uint32_t manager_id, uint64_t job_id);
int wm_reschedule_failed_job(uint32_t manager_id, uint64_t job_id);

uint64_t wm_get_total_jobs_submitted(uint32_t manager_id);
uint64_t wm_get_total_jobs_completed(uint32_t manager_id);
uint64_t wm_get_total_jobs_failed(uint32_t manager_id);
uint64_t wm_get_total_tasks_executed(uint32_t manager_id);

double wm_get_job_success_rate(uint32_t manager_id);
double wm_get_average_job_duration(uint32_t manager_id);
double wm_get_total_workload_cost(uint32_t manager_id);

int wm_get_workload_by_type(uint32_t manager_id, WorkloadType workload_type,
                            Job *jobs, uint32_t *count);
int wm_collect_workload_metrics(uint32_t manager_id);

WorkloadManager *wm_get_manager_info(uint32_t manager_id);

#endif
