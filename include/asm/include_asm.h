#ifndef INCLUDE_ASM_H
#define INCLUDE_ASM_H

#include <stdint.h>

typedef struct
{
    uint32_t reg_id;
    uint64_t value;
} include_asm_register_t;

typedef struct
{
    uint32_t insn_id;
    uint8_t opcode;
    uint64_t operand;
} include_asm_instruction_t;

int include_asm_read_register(uint32_t reg_id, uint64_t *value);
int include_asm_write_register(uint32_t reg_id, uint64_t value);
int include_asm_execute_instruction(include_asm_instruction_t *insn);
int include_asm_get_flags(uint32_t *flags);
int include_asm_set_flags(uint32_t flags);
int include_asm_get_pc(uint64_t *pc);

#endif
