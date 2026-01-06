#ifndef ARM64_H
#define ARM64_H

#include <stdint.h>

int arm64_init(void);
int arm64_setup_mmu(void);
int arm64_set_ttbr0(uint64_t ttbr0);
int arm64_enable_interrupts(void);
int arm64_disable_interrupts(void);
int arm64_context_switch(void *old_ctx, void *new_ctx);

#endif
