#include "serial_driver.h"

int serial_init(void)
{
    return 0;
}

int serial_open(serial_port_t *port)
{
    if (!port)
    {
        return -1;
    }
    return 0;
}

int serial_close(serial_port_t *port)
{
    if (!port)
    {
        return -1;
    }
    return 0;
}

int serial_configure(serial_port_t *port, serial_config_t *cfg)
{
    if (!port || !cfg)
    {
        return -1;
    }
    port->config = *cfg;
    return 0;
}

int serial_write(serial_port_t *port, uint8_t *data, uint32_t len)
{
    if (!port || !data || len == 0)
    {
        return -1;
    }
    return 0;
}

int serial_read(serial_port_t *port, uint8_t *data, uint32_t len)
{
    if (!port || !data || len == 0)
    {
        return -1;
    }
    return 0;
}
