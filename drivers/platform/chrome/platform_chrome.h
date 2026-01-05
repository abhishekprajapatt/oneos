#ifndef PLATFORM_CHROME_H
#define PLATFORM_CHROME_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    uint32_t subsys_id;
    uint32_t flags;
    uint32_t config_base;
} platform_chrome_device_t;

typedef struct
{
    uint8_t ec_version[32];
    uint32_t fw_version;
    uint32_t capabilities;
    uint8_t reset_flags;
} platform_chrome_ec_info_t;

int platform_chrome_init(void);
int platform_chrome_probe(platform_chrome_device_t *dev);
int platform_chrome_ec_command(platform_chrome_device_t *dev, uint8_t cmd, uint8_t *data, uint32_t len);
int platform_chrome_get_ec_version(platform_chrome_device_t *dev, platform_chrome_ec_info_t *info);
int platform_chrome_reboot(platform_chrome_device_t *dev, uint32_t reboot_flags);
int platform_chrome_get_boot_mode(platform_chrome_device_t *dev, uint32_t *mode);

#endif
