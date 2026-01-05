#include "input_driver.h"

int input_init(void)
{
    return 0;
}

int input_register_device(input_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int input_unregister_device(input_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int input_report_event(input_device_t *dev, input_event_t *event)
{
    if (!dev || !event)
    {
        return -1;
    }
    return 0;
}

int input_get_event(input_device_t *dev, input_event_t *event)
{
    if (!dev || !event)
    {
        return -1;
    }
    return 0;
}
