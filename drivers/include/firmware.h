#ifndef FIRMWARE_H
#define FIRMWARE_H

#include <stdint.h>

typedef struct
{
    const char *name;
    const uint8_t *data;
    uint32_t size;
} firmware_t;

typedef struct
{
    uint32_t fw_id;
    uint32_t version;
    uint32_t checksum;
    uint32_t flags;
} firmware_header_t;

#endif
