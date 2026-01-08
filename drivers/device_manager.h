#ifndef DEVICE_MANAGER_H
#define DEVICE_MANAGER_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_DEVICES 256

typedef enum
{
    DEVICE_STORAGE,
    DEVICE_NETWORK,
    DEVICE_AUDIO,
    DEVICE_VIDEO,
    DEVICE_INPUT,
    DEVICE_USB,
    DEVICE_MEMORY
} DeviceType;

typedef enum
{
    DEVICE_STATE_OFFLINE,
    DEVICE_STATE_ONLINE,
    DEVICE_STATE_ERROR,
    DEVICE_STATE_BUSY
} DeviceState;

typedef struct
{
    uint32_t device_id;
    char name[256];
    char vendor[256];
    char model[256];
    DeviceType type;
    DeviceState state;
    uint32_t driver_id;
    void (*init_fn)(void);
    void (*cleanup_fn)(void);
    int (*read_fn)(void *buffer, uint32_t size);
    int (*write_fn)(const void *buffer, uint32_t size);
} Device;

typedef struct
{
    Device devices[MAX_DEVICES];
    uint32_t device_count;
} DeviceManager;

DeviceManager *device_manager_create(void);
void device_manager_destroy(DeviceManager *dm);
uint32_t device_manager_register_device(DeviceManager *dm, const Device *device);
int device_manager_unregister_device(DeviceManager *dm, uint32_t device_id);
Device *device_manager_get_device(DeviceManager *dm, uint32_t device_id);
Device *device_manager_find_device_by_name(DeviceManager *dm, const char *name);
int device_manager_initialize_device(DeviceManager *dm, uint32_t device_id);
int device_manager_read_device(DeviceManager *dm, uint32_t device_id, void *buffer, uint32_t size);
int device_manager_write_device(DeviceManager *dm, uint32_t device_id, const void *buffer, uint32_t size);

#endif
