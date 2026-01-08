#include "device_manager.h"
#include <stdlib.h>
#include <string.h>

DeviceManager *device_manager_create(void)
{
    DeviceManager *dm = (DeviceManager *)malloc(sizeof(DeviceManager));
    if (!dm)
        return NULL;

    dm->device_count = 0;
    memset(dm->devices, 0, sizeof(dm->devices));

    return dm;
}

void device_manager_destroy(DeviceManager *dm)
{
    if (!dm)
        return;
    free(dm);
}

uint32_t device_manager_register_device(DeviceManager *dm, const Device *device)
{
    if (!dm || dm->device_count >= MAX_DEVICES || !device)
        return 0;

    Device *dev = &dm->devices[dm->device_count];
    memcpy(dev, device, sizeof(Device));
    dev->device_id = dm->device_count + 1;

    dm->device_count++;
    return dev->device_id;
}

int device_manager_unregister_device(DeviceManager *dm, uint32_t device_id)
{
    if (!dm)
        return -1;

    for (uint32_t i = 0; i < dm->device_count; i++)
    {
        if (dm->devices[i].device_id == device_id)
        {
            for (uint32_t j = i; j < dm->device_count - 1; j++)
            {
                dm->devices[j] = dm->devices[j + 1];
            }
            dm->device_count--;
            return 0;
        }
    }

    return -1;
}

Device *device_manager_get_device(DeviceManager *dm, uint32_t device_id)
{
    if (!dm)
        return NULL;

    for (uint32_t i = 0; i < dm->device_count; i++)
    {
        if (dm->devices[i].device_id == device_id)
        {
            return &dm->devices[i];
        }
    }

    return NULL;
}

Device *device_manager_find_device_by_name(DeviceManager *dm, const char *name)
{
    if (!dm || !name)
        return NULL;

    for (uint32_t i = 0; i < dm->device_count; i++)
    {
        if (strcmp(dm->devices[i].name, name) == 0)
        {
            return &dm->devices[i];
        }
    }

    return NULL;
}

int device_manager_initialize_device(DeviceManager *dm, uint32_t device_id)
{
    if (!dm)
        return -1;

    Device *dev = device_manager_get_device(dm, device_id);
    if (!dev)
        return -1;

    if (dev->init_fn)
    {
        dev->init_fn();
    }
    dev->state = DEVICE_STATE_ONLINE;

    return 0;
}

int device_manager_read_device(DeviceManager *dm, uint32_t device_id, void *buffer, uint32_t size)
{
    if (!dm || !buffer || size == 0)
        return -1;

    Device *dev = device_manager_get_device(dm, device_id);
    if (!dev || dev->state != DEVICE_STATE_ONLINE)
        return -1;

    if (dev->read_fn)
    {
        return dev->read_fn(buffer, size);
    }

    return -1;
}

int device_manager_write_device(DeviceManager *dm, uint32_t device_id, const void *buffer, uint32_t size)
{
    if (!dm || !buffer || size == 0)
        return -1;

    Device *dev = device_manager_get_device(dm, device_id);
    if (!dev || dev->state != DEVICE_STATE_ONLINE)
        return -1;

    if (dev->write_fn)
    {
        return dev->write_fn(buffer, size);
    }

    return -1;
}
