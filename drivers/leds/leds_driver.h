#ifndef LEDS_DRIVER_H
#define LEDS_DRIVER_H

#include <stdint.h>

typedef struct
{
    uint32_t id;
    const char *name;
    uint32_t brightness;
    uint32_t max_brightness;
} led_t;

int leds_init(void);
int leds_register(led_t *led);
int leds_unregister(uint32_t id);
int leds_set_brightness(uint32_t id, uint32_t brightness);
int leds_get_brightness(uint32_t id, uint32_t *brightness);

#endif
