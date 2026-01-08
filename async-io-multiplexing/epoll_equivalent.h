#ifndef EPOLL_EQUIVALENT_H
#define EPOLL_EQUIVALENT_H

#include <stdint.h>
#include <stddef.h>

#define MAX_EPOLL_FDS 65536
#define MAX_EPOLL_INSTANCES 64

typedef enum
{
    EPOLL_CLOEXEC = 0x80000000,
    EPOLL_LEVEL_TRIGGERED = 0,
    EPOLL_EDGE_TRIGGERED = 0x40000000
} EpollFlags;

typedef enum
{
    EPOLLIN = 0x001,
    EPOLLOUT = 0x004,
    EPOLLHUP = 0x010,
    EPOLLERR = 0x008,
    EPOLLPRI = 0x002,
    EPOLLRDNORM = 0x040,
    EPOLLWRNORM = 0x100
} EpollEvent;

typedef struct
{
    uint32_t events;
    void *ptr;
    uint32_t fd;
    uint64_t data;
} EpollEventData;

typedef struct
{
    int fd;
    uint32_t events;
    EpollEventData event_data;

    uint8_t is_edge_triggered;
    uint8_t is_oneshot;
    uint8_t in_rbtree;

    uint32_t event_count;
} EpollFdInfo;

typedef struct
{
    uint32_t epoll_fd;
    uint32_t fd_count;
    EpollFdInfo fds[MAX_EPOLL_FDS];

    EpollEventData ready_events[MAX_EPOLL_FDS];
    uint32_t ready_count;

    uint64_t total_polls;
    uint64_t total_ready_events;
    float poll_efficiency;
} EpollInstance;

typedef struct
{
    uint32_t manager_id;
    uint32_t epoll_instance_count;
    EpollInstance instances[MAX_EPOLL_INSTANCES];
} EpollManager;

EpollManager *epoll_manager_create(void);
int epoll_manager_destroy(EpollManager *manager);

int epoll_create(EpollManager *manager, uint32_t flags);
int epoll_create_instance(EpollManager *manager);

int epoll_ctl_add(EpollManager *manager, int epoll_fd, int fd, EpollEventData *event);
int epoll_ctl_mod(EpollManager *manager, int epoll_fd, int fd, EpollEventData *event);
int epoll_ctl_del(EpollManager *manager, int epoll_fd, int fd);

int epoll_wait(EpollManager *manager, int epoll_fd, EpollEventData *events,
               int maxevents, int timeout_ms);

int epoll_set_edge_triggered(EpollManager *manager, int epoll_fd, int fd, uint8_t edge_triggered);
int epoll_set_oneshot(EpollManager *manager, int epoll_fd, int fd, uint8_t oneshot);

typedef struct
{
    uint32_t manager_id;
    uint32_t epoll_instances;
    uint32_t total_monitored_fds;
    uint64_t total_polls;
    uint64_t total_events;
    float average_poll_efficiency;
} EpollMetrics;

int epoll_get_metrics(EpollManager *manager, EpollMetrics *metrics);

#endif
