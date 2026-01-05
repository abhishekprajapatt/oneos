#ifndef LEDS_RGB_H
#define LEDS_RGB_H

#include <stdint.h>

typedef struct
{
    uint32_t led_id;
    uint32_t base_address;
    uint8_t channel_count;
    uint32_t max_brightness;
} leds_rgb_device_t;

typedef struct
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint32_t brightness;
} leds_rgb_color_t;

int leds_rgb_init(void);
int leds_rgb_probe(leds_rgb_device_t *dev);
int leds_rgb_set_color(leds_rgb_device_t *dev, leds_rgb_color_t *color);
int leds_rgb_set_brightness(leds_rgb_device_t *dev, uint32_t brightness);
int leds_rgb_enable_pwm(leds_rgb_device_t *dev, uint32_t frequency);
int leds_rgb_disable(leds_rgb_device_t *dev);

#endif
