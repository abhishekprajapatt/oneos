#ifndef SYS_CLASS_H
#define SYS_CLASS_H

#include <stdint.h>

typedef struct
{
    uint32_t class_id;
    const char *class_name;
    uint32_t device_count;
    uint32_t max_devices;
} sys_class_entry_t;

typedef struct
{
    uint32_t total_classes;
    uint32_t total_devices;
    uint32_t class_instances;
} sys_class_state_t;

int sys_class_init(void);
int sys_class_register(sys_class_entry_t *class_entry);
int sys_class_unregister(uint32_t class_id);
int sys_class_add_device(uint32_t class_id, uint32_t device_id);
int sys_class_remove_device(uint32_t class_id, uint32_t device_id);
int sys_class_get_devices(uint32_t class_id, uint32_t *device_ids, uint32_t *count);
int sys_class_get_state(sys_class_state_t *state);

#endif
