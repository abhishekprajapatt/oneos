#include "gpu_driver.h"

int gpu_init(void)
{
    return 0;
}

int gpu_open(gpu_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int gpu_close(gpu_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int gpu_submit_command(gpu_device_t *dev, gpu_command_t *cmd)
{
    if (!dev || !cmd)
    {
        return -1;
    }
    return 0;
}

int gpu_wait_idle(gpu_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}
