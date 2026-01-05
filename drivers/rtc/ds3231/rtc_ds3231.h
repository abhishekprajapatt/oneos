#ifndef RTC_DS3231_H
#define RTC_DS3231_H

#include <stdint.h>

typedef struct
{
    uint32_t rtc_id;
    uint32_t base_address;
    uint8_t i2c_addr;
    uint32_t flags;
} rtc_ds3231_device_t;

typedef struct
{
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint16_t year;
} rtc_ds3231_time_t;

int rtc_ds3231_init(void);
int rtc_ds3231_probe(rtc_ds3231_device_t *dev);
int rtc_ds3231_get_time(rtc_ds3231_device_t *dev, rtc_ds3231_time_t *time);
int rtc_ds3231_set_time(rtc_ds3231_device_t *dev, rtc_ds3231_time_t *time);
int rtc_ds3231_enable_alarm(rtc_ds3231_device_t *dev, rtc_ds3231_time_t *alarm_time);
int rtc_ds3231_disable_alarm(rtc_ds3231_device_t *dev);

#endif
