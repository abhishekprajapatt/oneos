#include "ipc_service.h"

int ipc_service_init(void)
{
    return 0;
}

int ipc_service_create_channel(ipc_channel_t *channel)
{
    if (!channel)
    {
        return -1;
    }
    return 0;
}

int ipc_service_send_message(uint32_t channel_id, const void *message, uint32_t size)
{
    if (channel_id == 0 || !message || size == 0)
    {
        return -1;
    }
    return 0;
}

int ipc_service_receive_message(uint32_t channel_id, void *message, uint32_t *size)
{
    if (channel_id == 0 || !message || !size)
    {
        return -1;
    }
    return 0;
}

int ipc_service_subscribe(uint32_t channel_id, uint64_t pid)
{
    if (channel_id == 0 || pid == 0)
    {
        return -1;
    }
    return 0;
}

int ipc_service_unsubscribe(uint32_t channel_id, uint64_t pid)
{
    if (channel_id == 0 || pid == 0)
    {
        return -1;
    }
    return 0;
}

int ipc_service_delete_channel(uint32_t channel_id)
{
    if (channel_id == 0)
    {
        return -1;
    }
    return 0;
}
