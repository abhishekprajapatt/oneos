#ifndef INPUT_JOYSTICK_H
#define INPUT_JOYSTICK_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    uint32_t vendor_id;
    uint32_t product_id;
    uint8_t button_count;
    uint8_t axis_count;
} input_joystick_device_t;

typedef struct
{
    uint8_t button_state[16];
    int16_t axis[8];
    uint32_t timestamp;
} input_joystick_event_t;

int input_joystick_init(void);
int input_joystick_probe(input_joystick_device_t *dev);
int input_joystick_read_state(input_joystick_device_t *dev, input_joystick_event_t *event);
int input_joystick_set_rumble(input_joystick_device_t *dev, uint8_t strength, uint16_t duration);
int input_joystick_calibrate(input_joystick_device_t *dev);
int input_joystick_get_capabilities(input_joystick_device_t *dev, uint32_t *caps);

#endif
