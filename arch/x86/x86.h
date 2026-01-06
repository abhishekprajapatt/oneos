#ifndef X86_H
#define X86_H

#include <stdint.h>

int x86_init(void);
int x86_setup_gdt(void);
int x86_setup_idt(void);
int x86_enable_paging(void);
int x86_enable_interrupts(void);
int x86_disable_interrupts(void);
int x86_context_switch(void *old_ctx, void *new_ctx);

#endif
