#include "kernel.h"

static kernel_version_t kernel_version = {1, 0};
static uint64_t kernel_start_time = 0;

int kernel_init(void)
{
    kernel_start_time = 0;
    return 0;
}

int kernel_start(void)
{
    kernel_start_time = 1;
    return 0;
}

int kernel_shutdown(void)
{
    kernel_start_time = 0;
    return 0;
}

int kernel_get_version(kernel_version_t *version)
{
    if (!version)
    {
        return -1;
    }
    version->version = kernel_version.version;
    version->build_number = kernel_version.build_number;
    return 0;
}

int kernel_get_uptime(uint64_t *uptime)
{
    if (!uptime)
    {
        return -1;
    }
    *uptime = kernel_start_time;
    return 0;
}

int kernel_panic(const char *msg)
{
    if (!msg)
    {
        return -1;
    }
    return 0;
}
