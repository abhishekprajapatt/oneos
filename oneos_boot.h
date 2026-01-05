#ifndef ONEOS_BOOT_H
#define ONEOS_BOOT_H

#include <stdint.h>

typedef struct
{
    uint32_t boot_id;
    const char *bootloader_name;
    const char *version;
} oneos_boot_config_t;

int oneos_boot_init(void);
int oneos_boot_load_bootloader(void);
int oneos_boot_verify_firmware(void);
int oneos_boot_load_kernel(void);
int oneos_boot_initialize_hardware(void);
int oneos_boot_transfer_control(void);

#endif
