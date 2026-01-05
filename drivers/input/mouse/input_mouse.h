#ifndef INPUT_MOUSE_H
#define INPUT_MOUSE_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    uint32_t vendor_id;
    uint32_t product_id;
    uint8_t buttons;
    uint8_t resolution;
} input_mouse_device_t;

typedef struct
{
    int16_t dx;
    int16_t dy;
    int16_t dz;
    uint8_t button_state;
    uint32_t timestamp;
} input_mouse_event_t;

int input_mouse_init(void);
int input_mouse_probe(input_mouse_device_t *dev);
int input_mouse_read_event(input_mouse_device_t *dev, input_mouse_event_t *event);
int input_mouse_set_resolution(input_mouse_device_t *dev, uint8_t resolution);
int input_mouse_set_sample_rate(input_mouse_device_t *dev, uint8_t rate);
int input_mouse_get_id(input_mouse_device_t *dev, uint8_t *id);

#endif
