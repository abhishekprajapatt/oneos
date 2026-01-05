#include "rtc_ds3231.h"

int rtc_ds3231_init(void)
{
    return 0;
}

int rtc_ds3231_probe(rtc_ds3231_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int rtc_ds3231_get_time(rtc_ds3231_device_t *dev, rtc_ds3231_time_t *time)
{
    if (!dev || !time)
    {
        return -1;
    }
    return 0;
}

int rtc_ds3231_set_time(rtc_ds3231_device_t *dev, rtc_ds3231_time_t *time)
{
    if (!dev || !time)
    {
        return -1;
    }
    return 0;
}

int rtc_ds3231_enable_alarm(rtc_ds3231_device_t *dev, rtc_ds3231_time_t *alarm_time)
{
    if (!dev || !alarm_time)
    {
        return -1;
    }
    return 0;
}

int rtc_ds3231_disable_alarm(rtc_ds3231_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}
