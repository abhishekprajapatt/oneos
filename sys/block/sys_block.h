#ifndef SYS_BLOCK_H
#define SYS_BLOCK_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    const char *device_name;
    uint64_t block_size;
    uint64_t total_blocks;
    uint32_t device_type;
} sys_block_device_t;

typedef struct
{
    uint32_t block_device_count;
    uint64_t total_block_size;
    uint32_t active_devices;
    uint64_t read_ops;
    uint64_t write_ops;
} sys_block_state_t;

int sys_block_init(void);
int sys_block_register_device(sys_block_device_t *device);
int sys_block_unregister_device(uint32_t device_id);
int sys_block_read_block(uint32_t device_id, uint64_t block_num, void *buffer);
int sys_block_write_block(uint32_t device_id, uint64_t block_num, const void *buffer);
int sys_block_get_device_info(uint32_t device_id, sys_block_device_t *device);
int sys_block_get_state(sys_block_state_t *state);

#endif
