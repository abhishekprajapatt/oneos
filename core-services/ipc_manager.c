#include "ipc_manager.h"
#include <stdlib.h>
#include <string.h>

IPCManager *ipc_init(void)
{
    IPCManager *manager = (IPCManager *)malloc(sizeof(IPCManager));
    if (!manager)
        return NULL;

    memset(manager->pipes, 0, MAX_PIPES * sizeof(Pipe));
    memset(manager->queues, 0, MAX_MESSAGE_QUEUES * sizeof(MessageQueue));
    memset(manager->sockets, 0, MAX_SOCKETS * sizeof(Socket));

    manager->pipe_count = 0;
    manager->queue_count = 0;
    manager->socket_count = 0;
    manager->total_pipes_created = 0;
    manager->total_queues_created = 0;
    manager->total_sockets_created = 0;

    return manager;
}

void ipc_cleanup(IPCManager *manager)
{
    if (!manager)
        return;

    for (uint32_t i = 0; i < manager->pipe_count; i++)
    {
        if (manager->pipes[i].is_open)
        {
            manager->pipes[i].is_open = false;
        }
    }

    free(manager);
}

uint32_t ipc_create_pipe(IPCManager *manager, uint32_t reader_pid, uint32_t writer_pid)
{
    if (!manager || manager->pipe_count >= MAX_PIPES)
        return 0;

    Pipe *pipe = &manager->pipes[manager->pipe_count];
    pipe->pipe_id = manager->pipe_count + 1;
    pipe->reader_pid = reader_pid;
    pipe->writer_pid = writer_pid;
    pipe->buffer_pos = 0;
    pipe->buffer_size = 0;
    pipe->is_open = true;
    pipe->bytes_written = 0;
    pipe->bytes_read = 0;

    memset(pipe->buffer, 0, PIPE_BUFFER_SIZE);

    manager->pipe_count++;
    manager->total_pipes_created++;

    return pipe->pipe_id;
}

int ipc_pipe_write(IPCManager *manager, uint32_t pipe_id, const void *data, uint32_t size)
{
    if (!manager || !data || pipe_id == 0 || pipe_id > manager->pipe_count)
        return -1;

    Pipe *pipe = &manager->pipes[pipe_id - 1];
    if (!pipe->is_open)
        return -1;
    if (size + pipe->buffer_size > PIPE_BUFFER_SIZE)
        return -1;

    memcpy(&pipe->buffer[pipe->buffer_size], data, size);
    pipe->buffer_size += size;
    pipe->bytes_written += size;

    return size;
}

int ipc_pipe_read(IPCManager *manager, uint32_t pipe_id, void *buffer, uint32_t max_size)
{
    if (!manager || !buffer || pipe_id == 0 || pipe_id > manager->pipe_count)
        return -1;

    Pipe *pipe = &manager->pipes[pipe_id - 1];
    if (!pipe->is_open)
        return -1;
    if (pipe->buffer_size == 0)
        return 0;

    uint32_t read_size = (max_size < pipe->buffer_size) ? max_size : pipe->buffer_size;
    memcpy(buffer, pipe->buffer, read_size);

    memmove(pipe->buffer, &pipe->buffer[read_size], pipe->buffer_size - read_size);
    pipe->buffer_size -= read_size;
    pipe->bytes_read += read_size;

    return read_size;
}

int ipc_pipe_close(IPCManager *manager, uint32_t pipe_id)
{
    if (!manager || pipe_id == 0 || pipe_id > manager->pipe_count)
        return -1;

    Pipe *pipe = &manager->pipes[pipe_id - 1];
    pipe->is_open = false;
    pipe->buffer_size = 0;
    pipe->buffer_pos = 0;

    return 0;
}

uint32_t ipc_create_message_queue(IPCManager *manager, uint32_t owner_pid, uint32_t max_messages)
{
    if (!manager || manager->queue_count >= MAX_MESSAGE_QUEUES)
        return 0;

    MessageQueue *queue = &manager->queues[manager->queue_count];
    queue->queue_id = manager->queue_count + 1;
    queue->owner_pid = owner_pid;
    queue->message_count = 0;
    queue->max_messages = max_messages;
    queue->total_sent = 0;
    queue->total_received = 0;

    memset(queue->messages, 0, MAX_QUEUE_MESSAGES * sizeof(Message));

    manager->queue_count++;
    manager->total_queues_created++;

    return queue->queue_id;
}

int ipc_queue_send(IPCManager *manager, uint32_t queue_id, uint32_t sender_pid, const void *data, uint32_t size)
{
    if (!manager || !data || queue_id == 0 || queue_id > manager->queue_count)
        return -1;

    MessageQueue *queue = &manager->queues[queue_id - 1];
    if (queue->message_count >= queue->max_messages || size > MAX_MESSAGE_SIZE)
        return -1;

    Message *msg = &queue->messages[queue->message_count];
    msg->sender_pid = sender_pid;
    msg->size = size;
    msg->timestamp = time(NULL);
    memcpy(msg->data, data, size);

    queue->message_count++;
    queue->total_sent++;

    return size;
}

int ipc_queue_receive(IPCManager *manager, uint32_t queue_id, void *buffer, uint32_t max_size)
{
    if (!manager || !buffer || queue_id == 0 || queue_id > manager->queue_count)
        return -1;

    MessageQueue *queue = &manager->queues[queue_id - 1];
    if (queue->message_count == 0)
        return 0;

    Message *msg = &queue->messages[0];
    uint32_t copy_size = (max_size < msg->size) ? max_size : msg->size;

    memcpy(buffer, msg->data, copy_size);

    memmove(&queue->messages[0], &queue->messages[1], (queue->message_count - 1) * sizeof(Message));
    queue->message_count--;
    queue->total_received++;

    return copy_size;
}

int ipc_queue_delete(IPCManager *manager, uint32_t queue_id)
{
    if (!manager || queue_id == 0 || queue_id > manager->queue_count)
        return -1;

    MessageQueue *queue = &manager->queues[queue_id - 1];
    queue->message_count = 0;
    queue->owner_pid = 0;

    return 0;
}

uint32_t ipc_queue_get_message_count(IPCManager *manager, uint32_t queue_id)
{
    if (!manager || queue_id == 0 || queue_id > manager->queue_count)
        return 0;
    return manager->queues[queue_id - 1].message_count;
}

uint32_t ipc_create_socket(IPCManager *manager, SocketType type, SocketProtocol protocol, uint32_t owner_pid)
{
    if (!manager || manager->socket_count >= MAX_SOCKETS)
        return 0;

    Socket *socket = &manager->sockets[manager->socket_count];
    socket->socket_id = manager->socket_count + 1;
    socket->socket_type = type;
    socket->protocol = protocol;
    socket->owner_pid = owner_pid;
    socket->is_listening = false;
    socket->is_connected = false;
    socket->connected_socket_id = 0;
    socket->send_pos = 0;
    socket->recv_pos = 0;
    socket->bytes_sent = 0;
    socket->bytes_received = 0;

    memset(socket->send_buffer, 0, PIPE_BUFFER_SIZE);
    memset(socket->recv_buffer, 0, PIPE_BUFFER_SIZE);

    manager->socket_count++;
    manager->total_sockets_created++;

    return socket->socket_id;
}

int ipc_socket_bind(IPCManager *manager, uint32_t socket_id, const char *address)
{
    if (!manager || socket_id == 0 || socket_id > manager->socket_count || !address)
        return -1;

    Socket *socket = &manager->sockets[socket_id - 1];
    if (socket->is_listening || socket->is_connected)
        return -1;

    return 0;
}

int ipc_socket_listen(IPCManager *manager, uint32_t socket_id, uint32_t backlog)
{
    if (!manager || socket_id == 0 || socket_id > manager->socket_count)
        return -1;

    Socket *socket = &manager->sockets[socket_id - 1];
    if (socket->is_connected)
        return -1;

    socket->is_listening = true;
    return 0;
}

int ipc_socket_connect(IPCManager *manager, uint32_t socket_id, const char *address)
{
    if (!manager || socket_id == 0 || socket_id > manager->socket_count || !address)
        return -1;

    Socket *socket = &manager->sockets[socket_id - 1];
    if (socket->is_listening)
        return -1;

    socket->is_connected = true;
    return 0;
}

int ipc_socket_send(IPCManager *manager, uint32_t socket_id, const void *data, uint32_t size)
{
    if (!manager || !data || socket_id == 0 || socket_id > manager->socket_count)
        return -1;

    Socket *socket = &manager->sockets[socket_id - 1];
    if (!socket->is_connected)
        return -1;
    if (size + socket->send_pos > PIPE_BUFFER_SIZE)
        return -1;

    memcpy(&socket->send_buffer[socket->send_pos], data, size);
    socket->send_pos += size;
    socket->bytes_sent += size;

    return size;
}

int ipc_socket_recv(IPCManager *manager, uint32_t socket_id, void *buffer, uint32_t max_size)
{
    if (!manager || !buffer || socket_id == 0 || socket_id > manager->socket_count)
        return -1;

    Socket *socket = &manager->sockets[socket_id - 1];
    if (!socket->is_connected)
        return -1;
    if (socket->recv_pos == 0)
        return 0;

    uint32_t read_size = (max_size < socket->recv_pos) ? max_size : socket->recv_pos;
    memcpy(buffer, socket->recv_buffer, read_size);

    memmove(socket->recv_buffer, &socket->recv_buffer[read_size], socket->recv_pos - read_size);
    socket->recv_pos -= read_size;
    socket->bytes_received += read_size;

    return read_size;
}

int ipc_socket_close(IPCManager *manager, uint32_t socket_id)
{
    if (!manager || socket_id == 0 || socket_id > manager->socket_count)
        return -1;

    Socket *socket = &manager->sockets[socket_id - 1];
    socket->is_listening = false;
    socket->is_connected = false;
    socket->send_pos = 0;
    socket->recv_pos = 0;

    return 0;
}

uint32_t ipc_get_pipe_count(IPCManager *manager)
{
    if (!manager)
        return 0;
    return manager->pipe_count;
}

uint32_t ipc_get_queue_count(IPCManager *manager)
{
    if (!manager)
        return 0;
    return manager->queue_count;
}

uint32_t ipc_get_socket_count(IPCManager *manager)
{
    if (!manager)
        return 0;
    return manager->socket_count;
}
