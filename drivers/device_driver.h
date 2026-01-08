#ifndef DEVICE_DRIVER_H
#define DEVICE_DRIVER_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#define MAX_DRIVERS 512
#define MAX_DRIVER_INSTANCES 4096
#define MAX_DRIVER_NAME_LEN 64
#define MAX_DRIVER_OPS 32

typedef enum
{
    DEVICE_TYPE_BLOCK = 0,
    DEVICE_TYPE_CHAR = 1,
    DEVICE_TYPE_NETWORK = 2,
    DEVICE_TYPE_INPUT = 3,
    DEVICE_TYPE_USB = 4,
    DEVICE_TYPE_AUDIO = 5,
    DEVICE_TYPE_VIDEO = 6,
    DEVICE_TYPE_SERIAL = 7,
    DEVICE_TYPE_MISC = 8
} DeviceType;

typedef enum
{
    DRIVER_STATE_UNLOADED = 0,
    DRIVER_STATE_LOADING = 1,
    DRIVER_STATE_LOADED = 2,
    DRIVER_STATE_INITIALIZING = 3,
    DRIVER_STATE_INITIALIZED = 4,
    DRIVER_STATE_ACTIVE = 5,
    DRIVER_STATE_ERROR = 6,
    DRIVER_STATE_UNLOADING = 7
} DriverState;

typedef struct
{
    uint32_t device_id;
    DeviceType type;
    uint32_t major_num;
    uint32_t minor_num;
    char name[MAX_DRIVER_NAME_LEN];
    uint32_t driver_id;
    bool is_active;
    uint64_t bytes_read;
    uint64_t bytes_written;
    uint64_t operations;
    time_t created_time;
} DeviceInstance;

typedef struct
{
    int (*probe)(uint32_t device_id, void *data);
    int (*initialize)(uint32_t device_id);
    int (*cleanup)(uint32_t device_id);
    int (*read)(uint32_t device_id, void *buffer, uint32_t size);
    int (*write)(uint32_t device_id, const void *data, uint32_t size);
    int (*ioctl)(uint32_t device_id, uint32_t cmd, void *arg);
    int (*reset)(uint32_t device_id);
    int (*suspend)(uint32_t device_id);
    int (*resume)(uint32_t device_id);
} DriverOps;

typedef struct
{
    uint32_t driver_id;
    char name[MAX_DRIVER_NAME_LEN];
    char version[32];
    DeviceType device_type;
    DriverState state;
    DriverOps *ops;
    uint32_t device_count;
    uint64_t load_time;
    uint64_t total_operations;
    bool is_builtin;
} DriverDescriptor;

typedef struct
{
    DriverDescriptor drivers[MAX_DRIVERS];
    uint32_t driver_count;
    DeviceInstance instances[MAX_DRIVER_INSTANCES];
    uint32_t instance_count;
    uint64_t total_drivers_loaded;
} DriverManager;

DriverManager *driver_init(void);
void driver_cleanup(DriverManager *manager);

uint32_t driver_register(DriverManager *manager, const char *name, const char *version,
                         DeviceType type, DriverOps *ops, bool builtin);
int driver_unregister(DriverManager *manager, uint32_t driver_id);
int driver_load(DriverManager *manager, uint32_t driver_id);
int driver_unload(DriverManager *manager, uint32_t driver_id);

uint32_t device_create(DriverManager *manager, uint32_t driver_id, uint32_t major,
                       uint32_t minor, const char *name);
int device_delete(DriverManager *manager, uint32_t device_id);
int device_open(DriverManager *manager, uint32_t device_id);
int device_close(DriverManager *manager, uint32_t device_id);

int device_read(DriverManager *manager, uint32_t device_id, void *buffer, uint32_t size);
int device_write(DriverManager *manager, uint32_t device_id, const void *data, uint32_t size);
int device_ioctl(DriverManager *manager, uint32_t device_id, uint32_t cmd, void *arg);

DriverDescriptor *driver_get_info(DriverManager *manager, uint32_t driver_id);
DeviceInstance *device_get_info(DriverManager *manager, uint32_t device_id);
uint32_t driver_get_count(DriverManager *manager);
uint32_t device_get_count(DriverManager *manager);

#endif
