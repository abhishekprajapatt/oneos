#ifndef TOOLS_BPF_H
#define TOOLS_BPF_H

#include <stdint.h>

typedef struct
{
    uint32_t program_id;
    const char *program_name;
    const char *program_type;
    uint32_t instruction_count;
    uint32_t load_status;
} tools_bpf_program_t;

typedef struct
{
    uint32_t total_programs;
    uint32_t loaded_programs;
    uint32_t failed_programs;
    uint64_t total_instructions;
} tools_bpf_state_t;

int tools_bpf_init(void);
int tools_bpf_load_program(tools_bpf_program_t *program);
int tools_bpf_unload_program(uint32_t program_id);
int tools_bpf_verify_program(tools_bpf_program_t *program);
int tools_bpf_execute_program(uint32_t program_id, void *context);
int tools_bpf_get_state(tools_bpf_state_t *state);

#endif
