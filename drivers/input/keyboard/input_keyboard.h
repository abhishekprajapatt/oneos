#ifndef INPUT_KEYBOARD_H
#define INPUT_KEYBOARD_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    uint32_t vendor_id;
    uint32_t product_id;
    uint8_t layout;
    uint32_t flags;
} input_keyboard_device_t;

typedef struct
{
    uint32_t keycode;
    uint8_t pressed;
    uint8_t repeat_count;
    uint32_t timestamp;
} input_keyboard_event_t;

int input_keyboard_init(void);
int input_keyboard_probe(input_keyboard_device_t *dev);
int input_keyboard_read_event(input_keyboard_device_t *dev, input_keyboard_event_t *event);
int input_keyboard_set_leds(input_keyboard_device_t *dev, uint8_t leds);
int input_keyboard_get_layout(input_keyboard_device_t *dev, uint8_t *layout);
int input_keyboard_set_repeat_rate(input_keyboard_device_t *dev, uint8_t delay, uint8_t rate);

#endif
