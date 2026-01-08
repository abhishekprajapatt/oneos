#include "epoll_equivalent.h"
#include <stdlib.h>
#include <string.h>

static EpollManager *global_epoll_manager = NULL;
static uint32_t next_epoll_fd = 1;

EpollManager *epoll_manager_create(void)
{
    EpollManager *manager = (EpollManager *)malloc(sizeof(EpollManager));
    if (!manager)
        return NULL;

    memset(manager, 0, sizeof(EpollManager));
    manager->manager_id = 1;
    manager->epoll_instance_count = 0;

    global_epoll_manager = manager;
    return manager;
}

int epoll_manager_destroy(EpollManager *manager)
{
    if (!manager)
        return -1;

    free(manager);
    global_epoll_manager = NULL;
    return 0;
}

int epoll_create(EpollManager *manager, uint32_t flags)
{
    if (!manager)
        return -1;

    if (manager->epoll_instance_count >= MAX_EPOLL_INSTANCES)
        return -1;

    return epoll_create_instance(manager);
}

int epoll_create_instance(EpollManager *manager)
{
    if (!manager || manager->epoll_instance_count >= MAX_EPOLL_INSTANCES)
        return -1;

    EpollInstance *instance = &manager->instances[manager->epoll_instance_count];
    memset(instance, 0, sizeof(EpollInstance));

    instance->epoll_fd = next_epoll_fd++;
    instance->fd_count = 0;
    instance->ready_count = 0;
    instance->total_polls = 0;
    instance->total_ready_events = 0;
    instance->poll_efficiency = 0.0f;

    manager->epoll_instance_count++;
    return instance->epoll_fd;
}

int epoll_ctl_add(EpollManager *manager, int epoll_fd, int fd, EpollEventData *event)
{
    if (!manager || epoll_fd <= 0 || fd < 0 || !event)
        return -1;

    EpollInstance *instance = NULL;
    for (uint32_t i = 0; i < manager->epoll_instance_count; i++)
    {
        if (manager->instances[i].epoll_fd == epoll_fd)
        {
            instance = &manager->instances[i];
            break;
        }
    }

    if (!instance)
        return -1;

    if (instance->fd_count >= MAX_EPOLL_FDS)
        return -1;

    EpollFdInfo *fd_info = &instance->fds[instance->fd_count];
    memset(fd_info, 0, sizeof(EpollFdInfo));

    fd_info->fd = fd;
    fd_info->events = event->events;
    fd_info->event_data = *event;
    fd_info->is_edge_triggered = 0;
    fd_info->is_oneshot = 0;
    fd_info->in_rbtree = 1;
    fd_info->event_count = 0;

    instance->fd_count++;
    return 0;
}

int epoll_ctl_mod(EpollManager *manager, int epoll_fd, int fd, EpollEventData *event)
{
    if (!manager || epoll_fd <= 0 || fd < 0 || !event)
        return -1;

    EpollInstance *instance = NULL;
    for (uint32_t i = 0; i < manager->epoll_instance_count; i++)
    {
        if (manager->instances[i].epoll_fd == epoll_fd)
        {
            instance = &manager->instances[i];
            break;
        }
    }

    if (!instance)
        return -1;

    for (uint32_t i = 0; i < instance->fd_count; i++)
    {
        if (instance->fds[i].fd == fd)
        {
            instance->fds[i].events = event->events;
            instance->fds[i].event_data = *event;
            return 0;
        }
    }

    return -1;
}

int epoll_ctl_del(EpollManager *manager, int epoll_fd, int fd)
{
    if (!manager || epoll_fd <= 0 || fd < 0)
        return -1;

    EpollInstance *instance = NULL;
    for (uint32_t i = 0; i < manager->epoll_instance_count; i++)
    {
        if (manager->instances[i].epoll_fd == epoll_fd)
        {
            instance = &manager->instances[i];
            break;
        }
    }

    if (!instance)
        return -1;

    for (uint32_t i = 0; i < instance->fd_count; i++)
    {
        if (instance->fds[i].fd == fd)
        {
            memmove(&instance->fds[i], &instance->fds[i + 1],
                    sizeof(EpollFdInfo) * (instance->fd_count - i - 1));
            instance->fd_count--;
            return 0;
        }
    }

    return -1;
}

int epoll_wait(EpollManager *manager, int epoll_fd, EpollEventData *events,
               int maxevents, int timeout_ms)
{
    if (!manager || epoll_fd <= 0 || !events || maxevents <= 0)
        return -1;

    EpollInstance *instance = NULL;
    for (uint32_t i = 0; i < manager->epoll_instance_count; i++)
    {
        if (manager->instances[i].epoll_fd == epoll_fd)
        {
            instance = &manager->instances[i];
            break;
        }
    }

    if (!instance)
        return -1;

    instance->total_polls++;
    instance->ready_count = 0;

    for (uint32_t i = 0; i < instance->fd_count && instance->ready_count < (uint32_t)maxevents; i++)
    {
        EpollFdInfo *fd_info = &instance->fds[i];

        if (fd_info->events & EPOLLIN)
        {
            fd_info->event_count++;
            instance->ready_events[instance->ready_count] = fd_info->event_data;
            instance->ready_events[instance->ready_count].events = EPOLLIN;
            instance->ready_count++;
            instance->total_ready_events++;
        }
    }

    if (instance->total_polls > 0)
    {
        instance->poll_efficiency = (float)instance->total_ready_events / instance->total_polls;
    }

    uint32_t to_return = instance->ready_count < (uint32_t)maxevents ? instance->ready_count : maxevents;
    memcpy(events, instance->ready_events, sizeof(EpollEventData) * to_return);

    return to_return;
}

int epoll_set_edge_triggered(EpollManager *manager, int epoll_fd, int fd, uint8_t edge_triggered)
{
    if (!manager || epoll_fd <= 0 || fd < 0)
        return -1;

    EpollInstance *instance = NULL;
    for (uint32_t i = 0; i < manager->epoll_instance_count; i++)
    {
        if (manager->instances[i].epoll_fd == epoll_fd)
        {
            instance = &manager->instances[i];
            break;
        }
    }

    if (!instance)
        return -1;

    for (uint32_t i = 0; i < instance->fd_count; i++)
    {
        if (instance->fds[i].fd == fd)
        {
            instance->fds[i].is_edge_triggered = edge_triggered;
            if (edge_triggered)
            {
                instance->fds[i].events |= EPOLL_EDGE_TRIGGERED;
            }
            else
            {
                instance->fds[i].events &= ~EPOLL_EDGE_TRIGGERED;
            }
            return 0;
        }
    }

    return -1;
}

int epoll_set_oneshot(EpollManager *manager, int epoll_fd, int fd, uint8_t oneshot)
{
    if (!manager || epoll_fd <= 0 || fd < 0)
        return -1;

    EpollInstance *instance = NULL;
    for (uint32_t i = 0; i < manager->epoll_instance_count; i++)
    {
        if (manager->instances[i].epoll_fd == epoll_fd)
        {
            instance = &manager->instances[i];
            break;
        }
    }

    if (!instance)
        return -1;

    for (uint32_t i = 0; i < instance->fd_count; i++)
    {
        if (instance->fds[i].fd == fd)
        {
            instance->fds[i].is_oneshot = oneshot;
            return 0;
        }
    }

    return -1;
}

int epoll_get_metrics(EpollManager *manager, EpollMetrics *metrics)
{
    if (!manager || !metrics)
        return -1;

    metrics->manager_id = manager->manager_id;
    metrics->epoll_instances = manager->epoll_instance_count;

    uint32_t total_fds = 0;
    uint64_t total_polls = 0;
    uint64_t total_events = 0;
    float avg_efficiency = 0.0f;

    for (uint32_t i = 0; i < manager->epoll_instance_count; i++)
    {
        total_fds += manager->instances[i].fd_count;
        total_polls += manager->instances[i].total_polls;
        total_events += manager->instances[i].total_ready_events;
        avg_efficiency += manager->instances[i].poll_efficiency;
    }

    metrics->total_monitored_fds = total_fds;
    metrics->total_polls = total_polls;
    metrics->total_events = total_events;
    metrics->average_poll_efficiency = (manager->epoll_instance_count > 0) ? (avg_efficiency / manager->epoll_instance_count) : 0.0f;

    return 0;
}
