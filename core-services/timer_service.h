#ifndef TIMER_SERVICE_H
#define TIMER_SERVICE_H

#include <stdint.h>

typedef struct
{
    uint32_t timer_id;
    const char *timer_name;
    uint64_t interval_ms;
    uint32_t flags;
    uint64_t pid;
} timer_info_t;

int timer_service_init(void);
int timer_service_create_timer(timer_info_t *timer);
int timer_service_start_timer(uint32_t timer_id);
int timer_service_stop_timer(uint32_t timer_id);
int timer_service_delete_timer(uint32_t timer_id);
int timer_service_get_time(uint64_t *current_time);
int timer_service_set_alarm(uint64_t deadline, uint64_t pid);

#endif
