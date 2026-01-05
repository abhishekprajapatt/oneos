#ifndef SCSI_DRIVER_H
#define SCSI_DRIVER_H

#include <stdint.h>

typedef struct
{
    uint8_t target;
    uint8_t lun;
    uint32_t flags;
} scsi_device_t;

typedef struct
{
    uint8_t *cmd;
    uint32_t cmd_len;
    uint8_t *buffer;
    uint32_t buffer_len;
    uint32_t flags;
} scsi_request_t;

int scsi_init(void);
int scsi_scan(void);
int scsi_execute(scsi_device_t *dev, scsi_request_t *req);
int scsi_read_capacity(scsi_device_t *dev, uint64_t *capacity);

#endif
