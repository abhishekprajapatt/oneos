#ifndef INIT_H
#define INIT_H

#include <stdint.h>

typedef struct
{
    const char *name;
    void (*handler)(void);
} init_task_t;

int init_register_task(const char *name, void (*handler)(void));
int init_run_tasks(void);
int init_shutdown(void);
int init_get_phase(uint32_t *phase);
int init_set_phase(uint32_t phase);

#endif
