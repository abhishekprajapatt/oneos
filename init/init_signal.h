#ifndef INIT_SIGNAL_H
#define INIT_SIGNAL_H

#include <stdint.h>

typedef struct
{
    uint32_t signal_id;
    uint32_t signal_num;
    void (*handler)(uint32_t);
} init_signal_handler_t;

typedef struct
{
    uint32_t handler_count;
    uint32_t signal_count;
} init_signal_state_t;

int init_signal_register_handler(init_signal_handler_t *handler);
int init_signal_unregister_handler(uint32_t signal_id);
int init_signal_send_signal(uint32_t pid, uint32_t signal);
int init_signal_wait_signal(uint32_t *signal, uint32_t timeout);
int init_signal_block_signal(uint32_t signal);
int init_signal_unblock_signal(uint32_t signal);

#endif
