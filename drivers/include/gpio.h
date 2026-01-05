#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>

#define GPIO_DIR_INPUT 0
#define GPIO_DIR_OUTPUT 1

#define GPIO_PULL_NONE 0
#define GPIO_PULL_UP 1
#define GPIO_PULL_DOWN 2

#define GPIO_LEVEL_LOW 0
#define GPIO_LEVEL_HIGH 1

typedef struct
{
    uint32_t gpio_num;
    uint8_t direction;
    uint8_t pull_config;
    uint8_t level;
} gpio_config_t;

#endif
