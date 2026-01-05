#ifndef KERNEL_IPC_H
#define KERNEL_IPC_H

#include <stdint.h>

typedef struct
{
    uint32_t channel_id;
    uint32_t source_pid;
    uint32_t dest_pid;
    uint32_t message_size;
} kernel_ipc_message_t;

typedef struct
{
    uint32_t queue_id;
    uint32_t message_count;
    uint32_t max_messages;
} kernel_ipc_queue_t;

int kernel_ipc_init(void);
int kernel_ipc_create_queue(uint32_t max_messages, kernel_ipc_queue_t *queue);
int kernel_ipc_destroy_queue(uint32_t queue_id);
int kernel_ipc_send_message(kernel_ipc_message_t *message);
int kernel_ipc_receive_message(uint32_t queue_id, kernel_ipc_message_t *message);
int kernel_ipc_get_queue_status(uint32_t queue_id, uint32_t *count);

#endif
