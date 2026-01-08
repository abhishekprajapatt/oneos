#ifndef ASYNC_IO_MANAGER_H
#define ASYNC_IO_MANAGER_H

#include <stdint.h>
#include <stddef.h>

#define MAX_ASYNC_OPERATIONS 16384
#define MAX_COMPLETION_HANDLERS 256

typedef enum
{
    ASYNC_OP_READ,
    ASYNC_OP_WRITE,
    ASYNC_OP_FSYNC,
    ASYNC_OP_FSTAT,
    ASYNC_OP_OPEN,
    ASYNC_OP_CLOSE
} AsyncOpType;

typedef enum
{
    ASYNC_STATE_QUEUED,
    ASYNC_STATE_IN_PROGRESS,
    ASYNC_STATE_COMPLETED,
    ASYNC_STATE_FAILED,
    ASYNC_STATE_CANCELLED
} AsyncOpState;

typedef void (*CompletionCallback)(uint32_t op_id, AsyncOpState state, int32_t result, void *user_data);

typedef struct
{
    uint32_t async_op_id;
    AsyncOpType operation_type;
    AsyncOpState state;

    int fd;
    uint64_t offset;
    uint32_t length;
    uint8_t *buffer;

    CompletionCallback callback;
    void *user_data;

    int32_t result;
    uint32_t bytes_transferred;

    uint64_t submission_time;
    uint64_t completion_time;
    uint64_t latency_us;
} AsyncOperation;

typedef struct
{
    uint32_t handler_id;
    int fd;
    CompletionCallback callback;
    void *user_data;

    uint32_t completed_ops;
    uint32_t failed_ops;
} CompletionHandler;

typedef struct
{
    uint32_t manager_id;
    uint32_t operation_count;
    AsyncOperation operations[MAX_ASYNC_OPERATIONS];

    uint32_t handler_count;
    CompletionHandler handlers[MAX_COMPLETION_HANDLERS];

    uint32_t queued_ops;
    uint32_t in_progress_ops;
    uint32_t completed_ops;
    uint32_t failed_ops;

    uint64_t total_latency_us;
    float average_latency_us;

    uint8_t is_running;
} AsyncIOManager;

AsyncIOManager *async_io_init(void);
int async_io_destroy(AsyncIOManager *manager);

uint32_t async_submit_operation(AsyncIOManager *manager, AsyncOpType type, int fd,
                                uint64_t offset, uint32_t length, uint8_t *buffer,
                                CompletionCallback callback, void *user_data);
int async_cancel_operation(AsyncIOManager *manager, uint32_t op_id);

int async_poll_completions(AsyncIOManager *manager, uint32_t max_completions);
int async_wait_for_completion(AsyncIOManager *manager, uint32_t op_id, uint64_t timeout_us);

int async_register_handler(AsyncIOManager *manager, int fd, CompletionCallback callback, void *user_data);
int async_unregister_handler(AsyncIOManager *manager, int fd);

int async_get_operation_state(AsyncIOManager *manager, uint32_t op_id, AsyncOpState *state);
int async_get_operation_result(AsyncIOManager *manager, uint32_t op_id, int32_t *result);

typedef struct
{
    uint32_t manager_id;
    uint32_t queued_operations;
    uint32_t in_progress_operations;
    uint32_t completed_operations;
    uint32_t failed_operations;
    float average_latency_us;
    uint32_t active_handlers;
} AsyncIOMetrics;

int async_io_get_metrics(AsyncIOManager *manager, AsyncIOMetrics *metrics);

#endif
