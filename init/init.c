#include "init.h"

static uint32_t init_current_phase = 0;

int init_register_task(const char *name, void (*handler)(void))
{
    if (!name || !handler)
    {
        return -1;
    }
    return 0;
}

int init_run_tasks(void)
{
    init_current_phase = 1;
    return 0;
}

int init_shutdown(void)
{
    init_current_phase = 0;
    return 0;
}

int init_get_phase(uint32_t *phase)
{
    if (!phase)
    {
        return -1;
    }
    *phase = init_current_phase;
    return 0;
}

int init_set_phase(uint32_t phase)
{
    if (phase > 10)
    {
        return -1;
    }
    init_current_phase = phase;
    return 0;
}
