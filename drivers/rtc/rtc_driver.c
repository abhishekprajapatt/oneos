#include "rtc_driver.h"

int rtc_init(void)
{
    return 0;
}

int rtc_read_time(rtc_device_t *dev, rtc_time_t *time)
{
    if (!dev || !time)
    {
        return -1;
    }
    *time = dev->time;
    return 0;
}

int rtc_write_time(rtc_device_t *dev, rtc_time_t *time)
{
    if (!dev || !time)
    {
        return -1;
    }
    dev->time = *time;
    return 0;
}

int rtc_set_alarm(rtc_device_t *dev, rtc_time_t *alarm)
{
    if (!dev || !alarm)
    {
        return -1;
    }
    return 0;
}
