#ifndef DRIVER_MANAGER_H
#define DRIVER_MANAGER_H

#include <stdint.h>

typedef struct
{
    const char *driver_name;
    const char *driver_path;
    uint32_t driver_type;
    uint32_t status;
} driver_info_t;

int driver_manager_init(void);
int driver_manager_install_driver(const char *driver_file);
int driver_manager_remove_driver(const char *driver_name);
int driver_manager_start_driver(const char *driver_name);
int driver_manager_stop_driver(const char *driver_name);
int driver_manager_get_driver_info(const char *driver_name, driver_info_t *info);
int driver_manager_list_drivers(driver_info_t **drivers, uint32_t *count);

#endif
