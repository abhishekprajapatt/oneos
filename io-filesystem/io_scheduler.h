#ifndef IO_SCHEDULER_H
#define IO_SCHEDULER_H

#include <stdint.h>
#include <stddef.h>

#define MAX_IO_REQUESTS 4096
#define MAX_IO_QUEUES 32

typedef enum
{
    IO_READ,
    IO_WRITE,
    IO_TRIM,
    IO_FLUSH,
    IO_SYNC
} IOOperationType;

typedef enum
{
    IO_STATUS_PENDING,
    IO_STATUS_SCHEDULED,
    IO_STATUS_EXECUTING,
    IO_STATUS_COMPLETED,
    IO_STATUS_FAILED
} IORequestStatus;

typedef enum
{
    IO_SCHED_FIFO,
    IO_SCHED_ELEVATOR,
    IO_SCHED_DEADLINE,
    IO_SCHED_CFQ,
    IO_SCHED_NOOP
} IOSchedulingAlgorithm;

typedef struct
{
    uint32_t request_id;
    IOOperationType operation;
    uint64_t disk_sector;
    uint32_t sector_count;
    uint8_t *data_buffer;
    IORequestStatus status;

    uint64_t submission_time;
    uint64_t start_time;
    uint64_t completion_time;

    uint32_t priority;
    uint8_t is_queued;
} IORequest;

typedef struct
{
    uint32_t queue_id;
    uint32_t request_count;
    IORequest requests[1024];
    uint32_t front_index;
    uint32_t rear_index;

    uint64_t total_operations;
    uint64_t total_sectors_read;
    uint64_t total_sectors_written;
    uint64_t total_io_time_ms;
} IOQueue;

typedef struct
{
    uint32_t scheduler_id;
    IOSchedulingAlgorithm algorithm;

    uint32_t queue_count;
    IOQueue queues[MAX_IO_QUEUES];

    uint64_t total_requests;
    uint64_t total_read_operations;
    uint64_t total_write_operations;

    float average_io_latency_ms;
    float average_throughput_mbps;
    float disk_utilization;
} IOScheduler;

IOScheduler *io_scheduler_init(IOSchedulingAlgorithm algorithm, uint32_t queue_count);
int io_scheduler_destroy(IOScheduler *scheduler);

IORequest *io_submit_request(IOScheduler *scheduler, IOOperationType op, uint64_t sector, uint32_t count);
int io_cancel_request(IOScheduler *scheduler, uint32_t request_id);

int io_schedule_requests(IOScheduler *scheduler, uint32_t queue_id);
int io_elevator_sort(IOScheduler *scheduler, uint32_t queue_id, uint64_t current_head_position);
int io_deadline_sort(IOScheduler *scheduler, uint32_t queue_id);

int io_execute_request(IOScheduler *scheduler, IORequest *request);
int io_complete_request(IOScheduler *scheduler, uint32_t request_id);

int io_get_next_request(IOScheduler *scheduler, uint32_t queue_id, IORequest **request);

typedef struct
{
    uint32_t scheduler_id;
    uint64_t total_requests;
    uint64_t read_requests;
    uint64_t write_requests;
    float average_latency_ms;
    float throughput_mbps;
    float utilization_percent;
} IOMetrics;

int io_get_metrics(IOScheduler *scheduler, IOMetrics *metrics);

#endif
