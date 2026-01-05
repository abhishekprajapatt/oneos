#include "include_asm.h"

int include_asm_read_register(uint32_t reg_id, uint64_t *value)
{
    if (!value)
    {
        return -1;
    }
    return 0;
}

int include_asm_write_register(uint32_t reg_id, uint64_t value)
{
    return 0;
}

int include_asm_execute_instruction(include_asm_instruction_t *insn)
{
    if (!insn)
    {
        return -1;
    }
    return 0;
}

int include_asm_get_flags(uint32_t *flags)
{
    if (!flags)
    {
        return -1;
    }
    return 0;
}

int include_asm_set_flags(uint32_t flags)
{
    return 0;
}

int include_asm_get_pc(uint64_t *pc)
{
    if (!pc)
    {
        return -1;
    }
    return 0;
}
