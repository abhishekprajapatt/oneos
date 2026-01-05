#include "tools_bpf.h"

int tools_bpf_init(void)
{
    return 0;
}

int tools_bpf_load_program(tools_bpf_program_t *program)
{
    if (!program)
    {
        return -1;
    }
    return 0;
}

int tools_bpf_unload_program(uint32_t program_id)
{
    return 0;
}

int tools_bpf_verify_program(tools_bpf_program_t *program)
{
    if (!program)
    {
        return -1;
    }
    return 0;
}

int tools_bpf_execute_program(uint32_t program_id, void *context)
{
    if (!context)
    {
        return -1;
    }
    return 0;
}

int tools_bpf_get_state(tools_bpf_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
