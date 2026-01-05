#include "timer_service.h"

int timer_service_init(void)
{
    return 0;
}

int timer_service_create_timer(timer_info_t *timer)
{
    if (!timer)
    {
        return -1;
    }
    return 0;
}

int timer_service_start_timer(uint32_t timer_id)
{
    if (timer_id == 0)
    {
        return -1;
    }
    return 0;
}

int timer_service_stop_timer(uint32_t timer_id)
{
    if (timer_id == 0)
    {
        return -1;
    }
    return 0;
}

int timer_service_delete_timer(uint32_t timer_id)
{
    if (timer_id == 0)
    {
        return -1;
    }
    return 0;
}

int timer_service_get_time(uint64_t *current_time)
{
    if (!current_time)
    {
        return -1;
    }
    return 0;
}

int timer_service_set_alarm(uint64_t deadline, uint64_t pid)
{
    if (deadline == 0 || pid == 0)
    {
        return -1;
    }
    return 0;
}
