#ifndef INCLUDE_BITS_H
#define INCLUDE_BITS_H

#include <stdint.h>

typedef struct
{
    uint32_t bit_id;
    uint32_t position;
    uint8_t state;
} include_bits_field_t;

typedef struct
{
    uint32_t field_count;
    uint32_t total_bits;
} include_bits_state_t;

int include_bits_set_bit(uint32_t *bitmap, uint32_t position);
int include_bits_clear_bit(uint32_t *bitmap, uint32_t position);
int include_bits_test_bit(uint32_t *bitmap, uint32_t position);
int include_bits_toggle_bit(uint32_t *bitmap, uint32_t position);
int include_bits_find_first_set(uint32_t bitmap, uint32_t *position);
int include_bits_count_bits(uint32_t bitmap, uint32_t *count);

#endif
