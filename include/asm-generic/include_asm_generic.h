#ifndef INCLUDE_ASM_GENERIC_H
#define INCLUDE_ASM_GENERIC_H

#include <stdint.h>

typedef struct
{
    uint32_t offset;
    uint32_t size;
    uint32_t type;
} include_asm_generic_layout_t;

typedef struct
{
    uint32_t field_count;
    uint32_t total_size;
} include_asm_generic_state_t;

int include_asm_generic_get_layout(const char *struct_name, include_asm_generic_layout_t *layout);
int include_asm_generic_get_alignment(const char *struct_name, uint32_t *alignment);
int include_asm_generic_get_size(const char *struct_name, uint32_t *size);
int include_asm_generic_get_offset(const char *struct_name, const char *field_name, uint32_t *offset);
int include_asm_generic_validate_layout(const char *struct_name);
int include_asm_generic_dump_layout(const char *struct_name);

#endif
