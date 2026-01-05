#ifndef KERNEL_TIME_H
#define KERNEL_TIME_H

#include <stdint.h>

typedef struct
{
    uint64_t seconds;
    uint32_t nanoseconds;
} kernel_time_value_t;

typedef struct
{
    uint32_t timer_id;
    uint64_t expire_time;
    void (*callback)(uint32_t);
    uint32_t flags;
} kernel_time_timer_t;

int kernel_time_init(void);
int kernel_time_get_current(kernel_time_value_t *time);
int kernel_time_set_current(kernel_time_value_t *time);
int kernel_time_create_timer(kernel_time_timer_t *timer);
int kernel_time_delete_timer(uint32_t timer_id);
int kernel_time_get_resolution(uint32_t *resolution_ns);

#endif
