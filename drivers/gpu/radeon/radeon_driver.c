#include "radeon_driver.h"

int radeon_init(void)
{
    return 0;
}

int radeon_probe(void)
{
    return 0;
}

int radeon_open(radeon_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int radeon_close(radeon_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int radeon_buffer_create(radeon_device_t *dev, uint32_t size, radeon_buffer_t *buf)
{
    if (!dev || !buf || size == 0)
    {
        return -1;
    }
    return 0;
}

int radeon_buffer_destroy(radeon_device_t *dev, radeon_buffer_t *buf)
{
    if (!dev || !buf)
    {
        return -1;
    }
    return 0;
}

int radeon_cs_submit(radeon_device_t *dev, void *cs)
{
    if (!dev || !cs)
    {
        return -1;
    }
    return 0;
}
