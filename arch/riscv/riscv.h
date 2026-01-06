#ifndef RISCV_H
#define RISCV_H

#include <stdint.h>

int riscv_init(void);
int riscv_setup_trap_vector(uint64_t trap_vec);
int riscv_enable_interrupts(void);
int riscv_disable_interrupts(void);
int riscv_set_privilege_level(uint32_t level);
int riscv_context_switch(void *old_ctx, void *new_ctx);

#endif
