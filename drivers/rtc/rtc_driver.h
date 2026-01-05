#ifndef RTC_DRIVER_H
#define RTC_DRIVER_H

#include <stdint.h>

typedef struct
{
    uint32_t year;
    uint32_t month;
    uint32_t day;
    uint32_t hour;
    uint32_t minute;
    uint32_t second;
} rtc_time_t;

typedef struct
{
    uint32_t device_id;
    rtc_time_t time;
} rtc_device_t;

int rtc_init(void);
int rtc_read_time(rtc_device_t *dev, rtc_time_t *time);
int rtc_write_time(rtc_device_t *dev, rtc_time_t *time);
int rtc_set_alarm(rtc_device_t *dev, rtc_time_t *alarm);

#endif
