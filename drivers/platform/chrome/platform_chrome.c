#include "platform_chrome.h"

int platform_chrome_init(void)
{
    return 0;
}

int platform_chrome_probe(platform_chrome_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int platform_chrome_ec_command(platform_chrome_device_t *dev, uint8_t cmd, uint8_t *data, uint32_t len)
{
    if (!dev || !data)
    {
        return -1;
    }
    return 0;
}

int platform_chrome_get_ec_version(platform_chrome_device_t *dev, platform_chrome_ec_info_t *info)
{
    if (!dev || !info)
    {
        return -1;
    }
    return 0;
}

int platform_chrome_reboot(platform_chrome_device_t *dev, uint32_t reboot_flags)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int platform_chrome_get_boot_mode(platform_chrome_device_t *dev, uint32_t *mode)
{
    if (!dev || !mode)
    {
        return -1;
    }
    return 0;
}
