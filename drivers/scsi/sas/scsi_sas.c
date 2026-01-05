#include "scsi_sas.h"

int scsi_sas_init(void)
{
    return 0;
}

int scsi_sas_probe(scsi_sas_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int scsi_sas_enable(scsi_sas_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int scsi_sas_disable(scsi_sas_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int scsi_sas_send_smp_request(scsi_sas_device_t *dev, uint8_t *request, uint32_t req_len, uint8_t *response, uint32_t resp_len)
{
    if (!dev || !request || !response)
    {
        return -1;
    }
    return 0;
}

int scsi_sas_get_phy_info(scsi_sas_device_t *dev, uint8_t phy_id, scsi_sas_phy_info_t *info)
{
    if (!dev || !info)
    {
        return -1;
    }
    return 0;
}
