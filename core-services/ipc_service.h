#ifndef IPC_SERVICE_H
#define IPC_SERVICE_H

#include <stdint.h>

typedef struct
{
    uint32_t ipc_id;
    const char *channel_name;
    uint32_t type;
    uint32_t queue_size;
} ipc_channel_t;

int ipc_service_init(void);
int ipc_service_create_channel(ipc_channel_t *channel);
int ipc_service_send_message(uint32_t channel_id, const void *message, uint32_t size);
int ipc_service_receive_message(uint32_t channel_id, void *message, uint32_t *size);
int ipc_service_subscribe(uint32_t channel_id, uint64_t pid);
int ipc_service_unsubscribe(uint32_t channel_id, uint64_t pid);
int ipc_service_delete_channel(uint32_t channel_id);

#endif
