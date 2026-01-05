#include "serial_tty_driver.h"

int serial_tty_init(void)
{
    return 0;
}

int serial_tty_register_driver(serial_tty_info_t *info)
{
    if (!info)
    {
        return -1;
    }
    return 0;
}

int serial_tty_unregister_driver(uint32_t line)
{
    return 0;
}

int serial_tty_open(serial_tty_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int serial_tty_close(serial_tty_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int serial_tty_write(serial_tty_device_t *dev, uint8_t *data, uint32_t len)
{
    if (!dev || !data || len == 0)
    {
        return -1;
    }
    return 0;
}

int serial_tty_read(serial_tty_device_t *dev, uint8_t *data, uint32_t len)
{
    if (!dev || !data || len == 0)
    {
        return -1;
    }
    return 0;
}
