#ifndef INPUT_TOUCHPAD_H
#define INPUT_TOUCHPAD_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    uint32_t vendor_id;
    uint32_t product_id;
    uint32_t max_x;
    uint32_t max_y;
    uint8_t pressure_levels;
} input_touchpad_device_t;

typedef struct
{
    uint32_t x;
    uint32_t y;
    uint8_t pressure;
    uint8_t finger_count;
    uint32_t timestamp;
} input_touchpad_event_t;

int input_touchpad_init(void);
int input_touchpad_probe(input_touchpad_device_t *dev);
int input_touchpad_read_event(input_touchpad_device_t *dev, input_touchpad_event_t *event);
int input_touchpad_set_sensitivity(input_touchpad_device_t *dev, uint8_t sensitivity);
int input_touchpad_calibrate(input_touchpad_device_t *dev, uint32_t *min_x, uint32_t *max_x, uint32_t *min_y, uint32_t *max_y);
int input_touchpad_get_resolution(input_touchpad_device_t *dev, uint32_t *x_res, uint32_t *y_res);

#endif
