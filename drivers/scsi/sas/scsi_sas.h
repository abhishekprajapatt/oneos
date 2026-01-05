#ifndef SCSI_SAS_H
#define SCSI_SAS_H

#include <stdint.h>

typedef struct
{
    uint32_t sas_id;
    uint8_t sas_addr[8];
    uint32_t flags;
    uint32_t phy_count;
} scsi_sas_device_t;

typedef struct
{
    uint8_t phy_id;
    uint32_t linkrate;
    uint32_t negotiated_rate;
    uint8_t sas_address[8];
} scsi_sas_phy_info_t;

int scsi_sas_init(void);
int scsi_sas_probe(scsi_sas_device_t *dev);
int scsi_sas_enable(scsi_sas_device_t *dev);
int scsi_sas_disable(scsi_sas_device_t *dev);
int scsi_sas_send_smp_request(scsi_sas_device_t *dev, uint8_t *request, uint32_t req_len, uint8_t *response, uint32_t resp_len);
int scsi_sas_get_phy_info(scsi_sas_device_t *dev, uint8_t phy_id, scsi_sas_phy_info_t *info);

#endif
