#ifndef INPUT_DRIVER_H
#define INPUT_DRIVER_H

#include <stdint.h>

#define INPUT_EV_KEY 1
#define INPUT_EV_ABS 3
#define INPUT_EV_REL 2

typedef struct
{
    uint32_t type;
    uint32_t code;
    int32_t value;
    uint64_t timestamp;
} input_event_t;

typedef struct
{
    uint32_t device_id;
    uint32_t event_types;
} input_device_t;

int input_init(void);
int input_register_device(input_device_t *dev);
int input_unregister_device(input_device_t *dev);
int input_report_event(input_device_t *dev, input_event_t *event);
int input_get_event(input_device_t *dev, input_event_t *event);

#endif
