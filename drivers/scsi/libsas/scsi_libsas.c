#include "scsi_libsas.h"

int scsi_libsas_init(void)
{
    return 0;
}

int scsi_libsas_register_host(scsi_libsas_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int scsi_libsas_unregister_host(scsi_libsas_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int scsi_libsas_phy_enable(scsi_libsas_device_t *dev, uint8_t phy_id)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int scsi_libsas_phy_disable(scsi_libsas_device_t *dev, uint8_t phy_id)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int scsi_libsas_discover_domain(scsi_libsas_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}
