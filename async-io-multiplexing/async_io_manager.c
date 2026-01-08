#include "async_io_manager.h"
#include <stdlib.h>
#include <string.h>

static AsyncIOManager *global_async_io = NULL;
static uint32_t next_async_op_id = 1;

AsyncIOManager *async_io_init(void)
{
    AsyncIOManager *manager = (AsyncIOManager *)malloc(sizeof(AsyncIOManager));
    if (!manager)
        return NULL;

    memset(manager, 0, sizeof(AsyncIOManager));
    manager->manager_id = 1;
    manager->operation_count = 0;
    manager->handler_count = 0;
    manager->queued_ops = 0;
    manager->in_progress_ops = 0;
    manager->completed_ops = 0;
    manager->failed_ops = 0;
    manager->total_latency_us = 0;
    manager->average_latency_us = 0.0f;
    manager->is_running = 1;

    global_async_io = manager;
    return manager;
}

int async_io_destroy(AsyncIOManager *manager)
{
    if (!manager)
        return -1;

    for (uint32_t i = 0; i < manager->operation_count; i++)
    {
        if (manager->operations[i].buffer)
            free(manager->operations[i].buffer);
    }

    free(manager);
    global_async_io = NULL;
    return 0;
}

uint32_t async_submit_operation(AsyncIOManager *manager, AsyncOpType type, int fd,
                                uint64_t offset, uint32_t length, uint8_t *buffer,
                                CompletionCallback callback, void *user_data)
{
    if (!manager || fd < 0 || length == 0)
        return 0;

    if (manager->operation_count >= MAX_ASYNC_OPERATIONS)
        return 0;

    AsyncOperation *op = &manager->operations[manager->operation_count];
    memset(op, 0, sizeof(AsyncOperation));

    op->async_op_id = next_async_op_id++;
    op->operation_type = type;
    op->state = ASYNC_STATE_QUEUED;
    op->fd = fd;
    op->offset = offset;
    op->length = length;
    op->buffer = (uint8_t *)malloc(length);

    if (!op->buffer)
        return 0;

    if (buffer)
        memcpy(op->buffer, buffer, length);

    op->callback = callback;
    op->user_data = user_data;
    op->result = 0;
    op->bytes_transferred = 0;
    op->submission_time = 0;
    op->completion_time = 0;
    op->latency_us = 0;

    manager->operation_count++;
    manager->queued_ops++;

    return op->async_op_id;
}

int async_cancel_operation(AsyncIOManager *manager, uint32_t op_id)
{
    if (!manager || op_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->operation_count; i++)
    {
        if (manager->operations[i].async_op_id == op_id)
        {
            AsyncOperation *op = &manager->operations[i];

            if (op->state == ASYNC_STATE_QUEUED)
            {
                op->state = ASYNC_STATE_CANCELLED;
                manager->queued_ops--;
                return 0;
            }
            else if (op->state == ASYNC_STATE_IN_PROGRESS)
            {
                op->state = ASYNC_STATE_CANCELLED;
                manager->in_progress_ops--;
                return 0;
            }
        }
    }

    return -1;
}

int async_poll_completions(AsyncIOManager *manager, uint32_t max_completions)
{
    if (!manager || max_completions == 0)
        return -1;

    uint32_t completed = 0;

    for (uint32_t i = 0; i < manager->operation_count && completed < max_completions; i++)
    {
        AsyncOperation *op = &manager->operations[i];

        if (op->state == ASYNC_STATE_QUEUED)
        {
            op->state = ASYNC_STATE_IN_PROGRESS;
            manager->queued_ops--;
            manager->in_progress_ops++;
        }

        if (op->state == ASYNC_STATE_IN_PROGRESS)
        {
            op->completion_time = 0;
            op->bytes_transferred = op->length;
            op->result = 0;
            op->latency_us = (op->completion_time > op->submission_time) ? (op->completion_time - op->submission_time) : 100;

            op->state = ASYNC_STATE_COMPLETED;
            manager->in_progress_ops--;
            manager->completed_ops++;
            manager->total_latency_us += op->latency_us;

            if (op->callback)
            {
                op->callback(op->async_op_id, op->state, op->result, op->user_data);
            }

            completed++;
        }
    }

    if (manager->completed_ops > 0)
    {
        manager->average_latency_us = (float)manager->total_latency_us / manager->completed_ops;
    }

    return completed;
}

int async_wait_for_completion(AsyncIOManager *manager, uint32_t op_id, uint64_t timeout_us)
{
    if (!manager || op_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->operation_count; i++)
    {
        if (manager->operations[i].async_op_id == op_id)
        {
            AsyncOperation *op = &manager->operations[i];

            uint64_t elapsed = 0;
            while (elapsed < timeout_us && op->state != ASYNC_STATE_COMPLETED &&
                   op->state != ASYNC_STATE_FAILED && op->state != ASYNC_STATE_CANCELLED)
            {
                elapsed += 1000;
            }

            if (op->state == ASYNC_STATE_COMPLETED)
            {
                return op->result;
            }

            return -1;
        }
    }

    return -1;
}

int async_register_handler(AsyncIOManager *manager, int fd, CompletionCallback callback, void *user_data)
{
    if (!manager || fd < 0 || !callback)
        return -1;

    if (manager->handler_count >= MAX_COMPLETION_HANDLERS)
        return -1;

    CompletionHandler *handler = &manager->handlers[manager->handler_count];
    handler->handler_id = manager->handler_count + 1;
    handler->fd = fd;
    handler->callback = callback;
    handler->user_data = user_data;
    handler->completed_ops = 0;
    handler->failed_ops = 0;

    manager->handler_count++;
    return handler->handler_id;
}

int async_unregister_handler(AsyncIOManager *manager, int fd)
{
    if (!manager || fd < 0)
        return -1;

    for (uint32_t i = 0; i < manager->handler_count; i++)
    {
        if (manager->handlers[i].fd == fd)
        {
            memmove(&manager->handlers[i], &manager->handlers[i + 1],
                    sizeof(CompletionHandler) * (manager->handler_count - i - 1));
            manager->handler_count--;
            return 0;
        }
    }

    return -1;
}

int async_get_operation_state(AsyncIOManager *manager, uint32_t op_id, AsyncOpState *state)
{
    if (!manager || op_id == 0 || !state)
        return -1;

    for (uint32_t i = 0; i < manager->operation_count; i++)
    {
        if (manager->operations[i].async_op_id == op_id)
        {
            *state = manager->operations[i].state;
            return 0;
        }
    }

    return -1;
}

int async_get_operation_result(AsyncIOManager *manager, uint32_t op_id, int32_t *result)
{
    if (!manager || op_id == 0 || !result)
        return -1;

    for (uint32_t i = 0; i < manager->operation_count; i++)
    {
        if (manager->operations[i].async_op_id == op_id)
        {
            *result = manager->operations[i].result;
            return 0;
        }
    }

    return -1;
}

int async_io_get_metrics(AsyncIOManager *manager, AsyncIOMetrics *metrics)
{
    if (!manager || !metrics)
        return -1;

    metrics->manager_id = manager->manager_id;
    metrics->queued_operations = manager->queued_ops;
    metrics->in_progress_operations = manager->in_progress_ops;
    metrics->completed_operations = manager->completed_ops;
    metrics->failed_operations = manager->failed_ops;
    metrics->average_latency_us = manager->average_latency_us;
    metrics->active_handlers = manager->handler_count;

    return 0;
}
