#ifndef CLK_DRIVER_H
#define CLK_DRIVER_H

#include <stdint.h>

typedef struct
{
    uint32_t id;
    uint32_t rate;
    uint32_t parent_id;
    uint32_t flags;
} clk_t;

int clk_init(void);
int clk_register(clk_t *clk);
int clk_unregister(uint32_t id);
int clk_enable(uint32_t id);
int clk_disable(uint32_t id);
int clk_set_rate(uint32_t id, uint32_t rate);
uint32_t clk_get_rate(uint32_t id);

#endif
