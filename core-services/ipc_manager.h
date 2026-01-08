#ifndef IPC_MANAGER_H
#define IPC_MANAGER_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#define MAX_PIPES 1024
#define MAX_MESSAGE_QUEUES 512
#define MAX_SOCKETS 2048
#define MAX_PIPE_BUFFERS 4096
#define PIPE_BUFFER_SIZE 4096
#define MAX_MESSAGE_SIZE 8192
#define MAX_QUEUE_MESSAGES 1024

typedef enum
{
    PIPE_READ_END = 0,
    PIPE_WRITE_END = 1
} PipeEnd;

typedef enum
{
    SOCKET_UNIX = 0,
    SOCKET_INET = 1,
    SOCKET_INET6 = 2
} SocketType;

typedef enum
{
    SOCKET_STREAM = 0,
    SOCKET_DGRAM = 1,
    SOCKET_RAW = 2
} SocketProtocol;

typedef struct
{
    uint32_t pipe_id;
    uint32_t reader_pid;
    uint32_t writer_pid;
    uint8_t buffer[PIPE_BUFFER_SIZE];
    uint32_t buffer_pos;
    uint32_t buffer_size;
    bool is_open;
    uint64_t bytes_written;
    uint64_t bytes_read;
} Pipe;

typedef struct
{
    uint8_t data[MAX_MESSAGE_SIZE];
    uint32_t size;
    uint32_t sender_pid;
    time_t timestamp;
} Message;

typedef struct
{
    uint32_t queue_id;
    uint32_t owner_pid;
    Message messages[MAX_QUEUE_MESSAGES];
    uint32_t message_count;
    uint32_t max_messages;
    uint64_t total_sent;
    uint64_t total_received;
} MessageQueue;

typedef struct
{
    uint32_t socket_id;
    SocketType socket_type;
    SocketProtocol protocol;
    uint32_t owner_pid;
    bool is_listening;
    bool is_connected;
    uint32_t connected_socket_id;
    uint8_t send_buffer[PIPE_BUFFER_SIZE];
    uint8_t recv_buffer[PIPE_BUFFER_SIZE];
    uint32_t send_pos;
    uint32_t recv_pos;
    uint64_t bytes_sent;
    uint64_t bytes_received;
} Socket;

typedef struct
{
    Pipe pipes[MAX_PIPES];
    uint32_t pipe_count;
    MessageQueue queues[MAX_MESSAGE_QUEUES];
    uint32_t queue_count;
    Socket sockets[MAX_SOCKETS];
    uint32_t socket_count;
    uint64_t total_pipes_created;
    uint64_t total_queues_created;
    uint64_t total_sockets_created;
} IPCManager;

IPCManager *ipc_init(void);
void ipc_cleanup(IPCManager *manager);

uint32_t ipc_create_pipe(IPCManager *manager, uint32_t reader_pid, uint32_t writer_pid);
int ipc_pipe_write(IPCManager *manager, uint32_t pipe_id, const void *data, uint32_t size);
int ipc_pipe_read(IPCManager *manager, uint32_t pipe_id, void *buffer, uint32_t max_size);
int ipc_pipe_close(IPCManager *manager, uint32_t pipe_id);

uint32_t ipc_create_message_queue(IPCManager *manager, uint32_t owner_pid, uint32_t max_messages);
int ipc_queue_send(IPCManager *manager, uint32_t queue_id, uint32_t sender_pid, const void *data, uint32_t size);
int ipc_queue_receive(IPCManager *manager, uint32_t queue_id, void *buffer, uint32_t max_size);
int ipc_queue_delete(IPCManager *manager, uint32_t queue_id);
uint32_t ipc_queue_get_message_count(IPCManager *manager, uint32_t queue_id);

uint32_t ipc_create_socket(IPCManager *manager, SocketType type, SocketProtocol protocol, uint32_t owner_pid);
int ipc_socket_bind(IPCManager *manager, uint32_t socket_id, const char *address);
int ipc_socket_listen(IPCManager *manager, uint32_t socket_id, uint32_t backlog);
int ipc_socket_connect(IPCManager *manager, uint32_t socket_id, const char *address);
int ipc_socket_send(IPCManager *manager, uint32_t socket_id, const void *data, uint32_t size);
int ipc_socket_recv(IPCManager *manager, uint32_t socket_id, void *buffer, uint32_t max_size);
int ipc_socket_close(IPCManager *manager, uint32_t socket_id);

uint32_t ipc_get_pipe_count(IPCManager *manager);
uint32_t ipc_get_queue_count(IPCManager *manager);
uint32_t ipc_get_socket_count(IPCManager *manager);

#endif
