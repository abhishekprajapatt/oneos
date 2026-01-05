#include "scsi_driver.h"

int scsi_init(void)
{
    return 0;
}

int scsi_scan(void)
{
    return 0;
}

int scsi_execute(scsi_device_t *dev, scsi_request_t *req)
{
    if (!dev || !req)
    {
        return -1;
    }
    return 0;
}

int scsi_read_capacity(scsi_device_t *dev, uint64_t *capacity)
{
    if (!dev || !capacity)
    {
        return -1;
    }
    *capacity = 0;
    return 0;
}
