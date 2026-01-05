#ifndef REGMAP_H
#define REGMAP_H

#include <stdint.h>

typedef struct
{
    uint32_t reg_addr;
    uint32_t reg_value;
    uint32_t reg_mask;
} regmap_entry_t;

typedef struct
{
    uint32_t base_address;
    uint32_t size;
    uint32_t cache_type;
} regmap_config_t;

#define REGCACHE_NONE 0
#define REGCACHE_RBTREE 1
#define REGCACHE_FLAT 2

#endif
