#ifndef INPUT_SERIO_H
#define INPUT_SERIO_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    uint32_t port_base;
    uint8_t irq;
    uint32_t flags;
    uint32_t speed;
} input_serio_device_t;

typedef struct
{
    uint8_t data;
    uint32_t timestamp;
} input_serio_packet_t;

int input_serio_init(void);
int input_serio_probe(input_serio_device_t *dev);
int input_serio_write(input_serio_device_t *dev, uint8_t data);
int input_serio_read(input_serio_device_t *dev, uint8_t *data);
int input_serio_flush(input_serio_device_t *dev);
int input_serio_set_speed(input_serio_device_t *dev, uint32_t speed);

#endif
