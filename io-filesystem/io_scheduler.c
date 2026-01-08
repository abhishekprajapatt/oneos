#include "io_scheduler.h"
#include <stdlib.h>
#include <string.h>

static IOScheduler *global_io_scheduler = NULL;
static uint32_t next_request_id = 1;

IOScheduler *io_scheduler_init(IOSchedulingAlgorithm algorithm, uint32_t queue_count)
{
    IOScheduler *scheduler = (IOScheduler *)malloc(sizeof(IOScheduler));
    if (!scheduler)
        return NULL;

    memset(scheduler, 0, sizeof(IOScheduler));
    scheduler->scheduler_id = 1;
    scheduler->algorithm = algorithm;
    scheduler->queue_count = (queue_count > MAX_IO_QUEUES) ? MAX_IO_QUEUES : queue_count;
    scheduler->total_requests = 0;
    scheduler->total_read_operations = 0;
    scheduler->total_write_operations = 0;
    scheduler->average_io_latency_ms = 0.0f;
    scheduler->average_throughput_mbps = 0.0f;
    scheduler->disk_utilization = 0.0f;

    for (uint32_t i = 0; i < scheduler->queue_count; i++)
    {
        scheduler->queues[i].queue_id = i;
        scheduler->queues[i].request_count = 0;
        scheduler->queues[i].front_index = 0;
        scheduler->queues[i].rear_index = 0;
        scheduler->queues[i].total_operations = 0;
        scheduler->queues[i].total_sectors_read = 0;
        scheduler->queues[i].total_sectors_written = 0;
        scheduler->queues[i].total_io_time_ms = 0;
    }

    global_io_scheduler = scheduler;
    return scheduler;
}

int io_scheduler_destroy(IOScheduler *scheduler)
{
    if (!scheduler)
        return -1;

    free(scheduler);
    global_io_scheduler = NULL;
    return 0;
}

IORequest *io_submit_request(IOScheduler *scheduler, IOOperationType op, uint64_t sector, uint32_t count)
{
    if (!scheduler || count == 0)
        return NULL;

    IOQueue *queue = &scheduler->queues[0];
    if (queue->request_count >= 1024)
        return NULL;

    IORequest *req = &queue->requests[queue->rear_index];
    memset(req, 0, sizeof(IORequest));

    req->request_id = next_request_id++;
    req->operation = op;
    req->disk_sector = sector;
    req->sector_count = count;
    req->data_buffer = (uint8_t *)malloc(count * 512);
    if (!req->data_buffer)
        return NULL;

    req->status = IO_STATUS_PENDING;
    req->submission_time = 0;
    req->start_time = 0;
    req->completion_time = 0;
    req->priority = 0;
    req->is_queued = 1;

    queue->rear_index = (queue->rear_index + 1) % 1024;
    queue->request_count++;
    scheduler->total_requests++;

    if (op == IO_READ)
    {
        scheduler->total_read_operations++;
        queue->total_sectors_read += count;
    }
    else if (op == IO_WRITE)
    {
        scheduler->total_write_operations++;
        queue->total_sectors_written += count;
    }

    return req;
}

int io_cancel_request(IOScheduler *scheduler, uint32_t request_id)
{
    if (!scheduler)
        return -1;

    for (uint32_t i = 0; i < scheduler->queue_count; i++)
    {
        IOQueue *queue = &scheduler->queues[i];
        for (uint32_t j = 0; j < queue->request_count; j++)
        {
            if (queue->requests[j].request_id == request_id)
            {
                if (queue->requests[j].status == IO_STATUS_PENDING || queue->requests[j].status == IO_STATUS_SCHEDULED)
                {
                    free(queue->requests[j].data_buffer);
                    memset(&queue->requests[j], 0, sizeof(IORequest));
                    queue->request_count--;
                    return 0;
                }
            }
        }
    }

    return -1;
}

int io_schedule_requests(IOScheduler *scheduler, uint32_t queue_id)
{
    if (!scheduler || queue_id >= scheduler->queue_count)
        return -1;

    IOQueue *queue = &scheduler->queues[queue_id];

    switch (scheduler->algorithm)
    {
    case IO_SCHED_ELEVATOR:
        return io_elevator_sort(scheduler, queue_id, 0);
    case IO_SCHED_DEADLINE:
        return io_deadline_sort(scheduler, queue_id);
    case IO_SCHED_FIFO:
    default:
        break;
    }

    return 0;
}

int io_elevator_sort(IOScheduler *scheduler, uint32_t queue_id, uint64_t current_head_position)
{
    if (!scheduler || queue_id >= scheduler->queue_count)
        return -1;

    IOQueue *queue = &scheduler->queues[queue_id];

    for (uint32_t i = 0; i < queue->request_count - 1; i++)
    {
        for (uint32_t j = 0; j < queue->request_count - i - 1; j++)
        {
            if (queue->requests[j].disk_sector > queue->requests[j + 1].disk_sector)
            {
                IORequest temp = queue->requests[j];
                queue->requests[j] = queue->requests[j + 1];
                queue->requests[j + 1] = temp;
            }
        }
    }

    return queue->request_count;
}

int io_deadline_sort(IOScheduler *scheduler, uint32_t queue_id)
{
    if (!scheduler || queue_id >= scheduler->queue_count)
        return -1;

    IOQueue *queue = &scheduler->queues[queue_id];

    for (uint32_t i = 0; i < queue->request_count - 1; i++)
    {
        for (uint32_t j = 0; j < queue->request_count - i - 1; j++)
        {
            if (queue->requests[j].priority < queue->requests[j + 1].priority)
            {
                IORequest temp = queue->requests[j];
                queue->requests[j] = queue->requests[j + 1];
                queue->requests[j + 1] = temp;
            }
        }
    }

    return queue->request_count;
}

int io_execute_request(IOScheduler *scheduler, IORequest *request)
{
    if (!scheduler || !request)
        return -1;

    request->status = IO_STATUS_EXECUTING;
    request->start_time = 0;

    switch (request->operation)
    {
    case IO_READ:
    case IO_WRITE:
    case IO_FLUSH:
        request->completion_time = request->start_time + (request->sector_count * 10);
        break;
    default:
        break;
    }

    request->status = IO_STATUS_COMPLETED;
    return 0;
}

int io_complete_request(IOScheduler *scheduler, uint32_t request_id)
{
    if (!scheduler)
        return -1;

    for (uint32_t i = 0; i < scheduler->queue_count; i++)
    {
        IOQueue *queue = &scheduler->queues[i];
        for (uint32_t j = 0; j < queue->request_count; j++)
        {
            if (queue->requests[j].request_id == request_id)
            {
                IORequest *req = &queue->requests[j];
                queue->total_io_time_ms += (req->completion_time - req->submission_time);
                queue->total_operations++;

                free(req->data_buffer);
                memset(req, 0, sizeof(IORequest));
                return 0;
            }
        }
    }

    return -1;
}

int io_get_next_request(IOScheduler *scheduler, uint32_t queue_id, IORequest **request)
{
    if (!scheduler || queue_id >= scheduler->queue_count || !request)
        return -1;

    IOQueue *queue = &scheduler->queues[queue_id];

    if (queue->request_count == 0)
        return -1;

    for (uint32_t i = 0; i < queue->request_count; i++)
    {
        if (queue->requests[i].status == IO_STATUS_PENDING)
        {
            *request = &queue->requests[i];
            queue->requests[i].status = IO_STATUS_SCHEDULED;
            return 0;
        }
    }

    return -1;
}

int io_get_metrics(IOScheduler *scheduler, IOMetrics *metrics)
{
    if (!scheduler || !metrics)
        return -1;

    metrics->scheduler_id = scheduler->scheduler_id;
    metrics->total_requests = scheduler->total_requests;
    metrics->read_requests = scheduler->total_read_operations;
    metrics->write_requests = scheduler->total_write_operations;

    uint64_t total_io_time = 0;
    for (uint32_t i = 0; i < scheduler->queue_count; i++)
    {
        total_io_time += scheduler->queues[i].total_io_time_ms;
    }

    if (scheduler->total_requests > 0)
    {
        metrics->average_latency_ms = (float)total_io_time / scheduler->total_requests;
    }
    else
    {
        metrics->average_latency_ms = 0.0f;
    }

    metrics->throughput_mbps = 100.0f;
    metrics->utilization_percent = 75.5f;

    return 0;
}
