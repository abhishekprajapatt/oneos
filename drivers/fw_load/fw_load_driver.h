#ifndef FW_LOAD_DRIVER_H
#define FW_LOAD_DRIVER_H

#include <stdint.h>

typedef struct
{
    const char *name;
    uint8_t *data;
    uint32_t size;
} firmware_t;

int fw_init(void);
int fw_load(const char *name, firmware_t *fw);
int fw_unload(firmware_t *fw);
int fw_request_firmware(const char *name, firmware_t *fw);

#endif
