#include "fw_load_driver.h"

int fw_init(void)
{
    return 0;
}

int fw_load(const char *name, firmware_t *fw)
{
    if (!name || !fw)
    {
        return -1;
    }
    return 0;
}

int fw_unload(firmware_t *fw)
{
    if (!fw)
    {
        return -1;
    }
    return 0;
}

int fw_request_firmware(const char *name, firmware_t *fw)
{
    if (!name || !fw)
    {
        return -1;
    }
    return 0;
}
