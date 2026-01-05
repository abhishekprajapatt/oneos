#ifndef SCSI_LIBSAS_H
#define SCSI_LIBSAS_H

#include <stdint.h>

typedef struct
{
    uint32_t libsas_id;
    uint32_t flags;
    uint32_t max_phy;
    uint32_t phy_mask;
} scsi_libsas_device_t;

typedef struct
{
    uint8_t phy_id;
    uint32_t linkrate;
    uint8_t attached_addr[8];
    uint32_t phy_state;
} scsi_libsas_phy_t;

int scsi_libsas_init(void);
int scsi_libsas_register_host(scsi_libsas_device_t *dev);
int scsi_libsas_unregister_host(scsi_libsas_device_t *dev);
int scsi_libsas_phy_enable(scsi_libsas_device_t *dev, uint8_t phy_id);
int scsi_libsas_phy_disable(scsi_libsas_device_t *dev, uint8_t phy_id);
int scsi_libsas_discover_domain(scsi_libsas_device_t *dev);

#endif
