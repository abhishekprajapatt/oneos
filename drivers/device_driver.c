#include "device_driver.h"
#include <stdlib.h>
#include <string.h>

DriverManager *driver_init(void)
{
    DriverManager *manager = (DriverManager *)malloc(sizeof(DriverManager));
    if (!manager)
        return NULL;

    memset(manager->drivers, 0, MAX_DRIVERS * sizeof(DriverDescriptor));
    memset(manager->instances, 0, MAX_DRIVER_INSTANCES * sizeof(DeviceInstance));

    manager->driver_count = 0;
    manager->instance_count = 0;
    manager->total_drivers_loaded = 0;

    return manager;
}

void driver_cleanup(DriverManager *manager)
{
    if (!manager)
        return;

    for (uint32_t i = 0; i < manager->driver_count; i++)
    {
        if (manager->drivers[i].state != DRIVER_STATE_UNLOADED)
        {
            driver_unload(manager, manager->drivers[i].driver_id);
        }
    }

    free(manager);
}

uint32_t driver_register(DriverManager *manager, const char *name, const char *version,
                         DeviceType type, DriverOps *ops, bool builtin)
{
    if (!manager || !name || !ops || manager->driver_count >= MAX_DRIVERS)
        return 0;

    DriverDescriptor *desc = &manager->drivers[manager->driver_count];
    desc->driver_id = manager->driver_count + 1;

    strncpy(desc->name, name, MAX_DRIVER_NAME_LEN - 1);
    desc->name[MAX_DRIVER_NAME_LEN - 1] = '\0';

    strncpy(desc->version, version, 31);
    desc->version[31] = '\0';

    desc->device_type = type;
    desc->state = DRIVER_STATE_UNLOADED;
    desc->ops = ops;
    desc->device_count = 0;
    desc->load_time = 0;
    desc->total_operations = 0;
    desc->is_builtin = builtin;

    manager->driver_count++;
    return desc->driver_id;
}

int driver_unregister(DriverManager *manager, uint32_t driver_id)
{
    if (!manager || driver_id == 0 || driver_id > manager->driver_count)
        return -1;

    DriverDescriptor *desc = &manager->drivers[driver_id - 1];
    if (desc->device_count > 0)
        return -1;

    driver_unload(manager, driver_id);
    memset(desc, 0, sizeof(DriverDescriptor));

    return 0;
}

int driver_load(DriverManager *manager, uint32_t driver_id)
{
    if (!manager || driver_id == 0 || driver_id > manager->driver_count)
        return -1;

    DriverDescriptor *desc = &manager->drivers[driver_id - 1];
    if (desc->state != DRIVER_STATE_UNLOADED)
        return -1;

    desc->state = DRIVER_STATE_LOADING;
    desc->load_time = time(NULL);
    desc->state = DRIVER_STATE_LOADED;

    manager->total_drivers_loaded++;

    return 0;
}

int driver_unload(DriverManager *manager, uint32_t driver_id)
{
    if (!manager || driver_id == 0 || driver_id > manager->driver_count)
        return -1;

    DriverDescriptor *desc = &manager->drivers[driver_id - 1];
    if (desc->state == DRIVER_STATE_UNLOADED)
        return -1;

    desc->state = DRIVER_STATE_UNLOADING;
    desc->state = DRIVER_STATE_UNLOADED;
    desc->device_count = 0;

    return 0;
}

uint32_t device_create(DriverManager *manager, uint32_t driver_id, uint32_t major,
                       uint32_t minor, const char *name)
{
    if (!manager || !name || driver_id == 0 || driver_id > manager->driver_count)
        return 0;
    if (manager->instance_count >= MAX_DRIVER_INSTANCES)
        return 0;

    DriverDescriptor *desc = &manager->drivers[driver_id - 1];
    if (desc->state != DRIVER_STATE_LOADED)
        return 0;

    DeviceInstance *inst = &manager->instances[manager->instance_count];
    inst->device_id = manager->instance_count + 1;
    inst->type = desc->device_type;
    inst->major_num = major;
    inst->minor_num = minor;
    inst->driver_id = driver_id;
    inst->is_active = false;
    inst->bytes_read = 0;
    inst->bytes_written = 0;
    inst->operations = 0;
    inst->created_time = time(NULL);

    strncpy(inst->name, name, MAX_DRIVER_NAME_LEN - 1);
    inst->name[MAX_DRIVER_NAME_LEN - 1] = '\0';

    desc->device_count++;
    manager->instance_count++;

    return inst->device_id;
}

int device_delete(DriverManager *manager, uint32_t device_id)
{
    if (!manager || device_id == 0 || device_id > manager->instance_count)
        return -1;

    DeviceInstance *inst = &manager->instances[device_id - 1];
    if (inst->is_active)
        return -1;

    DriverDescriptor *desc = &manager->drivers[inst->driver_id - 1];
    if (desc->device_count > 0)
        desc->device_count--;

    memset(inst, 0, sizeof(DeviceInstance));

    return 0;
}

int device_open(DriverManager *manager, uint32_t device_id)
{
    if (!manager || device_id == 0 || device_id > manager->instance_count)
        return -1;

    DeviceInstance *inst = &manager->instances[device_id - 1];
    if (inst->is_active)
        return -1;

    DriverDescriptor *desc = &manager->drivers[inst->driver_id - 1];
    if (!desc->ops || !desc->ops->initialize)
        return -1;

    if (desc->ops->initialize(device_id) != 0)
        return -1;

    inst->is_active = true;
    return 0;
}

int device_close(DriverManager *manager, uint32_t device_id)
{
    if (!manager || device_id == 0 || device_id > manager->instance_count)
        return -1;

    DeviceInstance *inst = &manager->instances[device_id - 1];
    if (!inst->is_active)
        return -1;

    DriverDescriptor *desc = &manager->drivers[inst->driver_id - 1];
    if (desc->ops && desc->ops->cleanup)
    {
        desc->ops->cleanup(device_id);
    }

    inst->is_active = false;
    return 0;
}

int device_read(DriverManager *manager, uint32_t device_id, void *buffer, uint32_t size)
{
    if (!manager || !buffer || device_id == 0 || device_id > manager->instance_count)
        return -1;

    DeviceInstance *inst = &manager->instances[device_id - 1];
    if (!inst->is_active)
        return -1;

    DriverDescriptor *desc = &manager->drivers[inst->driver_id - 1];
    if (!desc->ops || !desc->ops->read)
        return -1;

    int result = desc->ops->read(device_id, buffer, size);
    if (result > 0)
    {
        inst->bytes_read += result;
        inst->operations++;
        desc->total_operations++;
    }

    return result;
}

int device_write(DriverManager *manager, uint32_t device_id, const void *data, uint32_t size)
{
    if (!manager || !data || device_id == 0 || device_id > manager->instance_count)
        return -1;

    DeviceInstance *inst = &manager->instances[device_id - 1];
    if (!inst->is_active)
        return -1;

    DriverDescriptor *desc = &manager->drivers[inst->driver_id - 1];
    if (!desc->ops || !desc->ops->write)
        return -1;

    int result = desc->ops->write(device_id, data, size);
    if (result > 0)
    {
        inst->bytes_written += result;
        inst->operations++;
        desc->total_operations++;
    }

    return result;
}

int device_ioctl(DriverManager *manager, uint32_t device_id, uint32_t cmd, void *arg)
{
    if (!manager || device_id == 0 || device_id > manager->instance_count)
        return -1;

    DeviceInstance *inst = &manager->instances[device_id - 1];
    if (!inst->is_active)
        return -1;

    DriverDescriptor *desc = &manager->drivers[inst->driver_id - 1];
    if (!desc->ops || !desc->ops->ioctl)
        return -1;

    int result = desc->ops->ioctl(device_id, cmd, arg);
    if (result == 0)
    {
        inst->operations++;
        desc->total_operations++;
    }

    return result;
}

DriverDescriptor *driver_get_info(DriverManager *manager, uint32_t driver_id)
{
    if (!manager || driver_id == 0 || driver_id > manager->driver_count)
        return NULL;
    return &manager->drivers[driver_id - 1];
}

DeviceInstance *device_get_info(DriverManager *manager, uint32_t device_id)
{
    if (!manager || device_id == 0 || device_id > manager->instance_count)
        return NULL;
    return &manager->instances[device_id - 1];
}

uint32_t driver_get_count(DriverManager *manager)
{
    if (!manager)
        return 0;
    return manager->driver_count;
}

uint32_t device_get_count(DriverManager *manager)
{
    if (!manager)
        return 0;
    return manager->instance_count;
}
