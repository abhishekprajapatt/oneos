#include "cocoa.h"

int cocoa_init(void)
{
    return 0;
}

int cocoa_run_application(void)
{
    return 0;
}

int cocoa_process_events(void)
{
    return 0;
}

int cocoa_get_main_thread(uint64_t *thread_id)
{
    if (!thread_id)
    {
        return -1;
    }
    *thread_id = 0;
    return 0;
}

int cocoa_dispatch_on_main(void (*callback)(void))
{
    if (!callback)
    {
        return -1;
    }
    return 0;
}

int cocoa_shutdown(void)
{
    return 0;
}
