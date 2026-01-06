#include "libasan.h"

int libasan_init(void)
{
    return 0;
}

int libasan_detect_memory_error(void *addr, uint32_t size)
{
    if (!addr || size == 0)
    {
        return -1;
    }
    return 0;
}

int libasan_report_error(const char *error_message)
{
    if (!error_message)
    {
        return -1;
    }
    return 0;
}

int libasan_enable_instrumentation(void)
{
    return 0;
}

int libasan_disable_instrumentation(void)
{
    return 0;
}

int libasan_get_status(void)
{
    return 0;
}
