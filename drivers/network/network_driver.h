#ifndef NETWORK_DRIVER_H
#define NETWORK_DRIVER_H

#include <stdint.h>

typedef struct
{
    uint32_t ipv4_addr;
    uint32_t ipv4_mask;
    uint32_t ipv4_gw;
} network_config_t;

typedef struct
{
    uint32_t device_id;
    network_config_t config;
    uint32_t flags;
} network_device_t;

int network_init(void);
int network_configure(network_device_t *dev, network_config_t *cfg);
int network_up(network_device_t *dev);
int network_down(network_device_t *dev);
int network_get_config(network_device_t *dev, network_config_t *cfg);

#endif
