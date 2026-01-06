#ifndef APT_MANAGER_H
#define APT_MANAGER_H

#include <stdint.h>

typedef struct
{
    const char *package_name;
    const char *version;
    const char *description;
} apt_package_t;

int apt_manager_init(void);
int apt_manager_update_repos(void);
int apt_manager_search_package(const char *package_name, apt_package_t *pkg);
int apt_manager_install_package(const char *package_name, const char *version);
int apt_manager_remove_package(const char *package_name);
int apt_manager_upgrade_all(void);
int apt_manager_get_installed(apt_package_t **packages, uint32_t *count);
int apt_manager_add_ppa(const char *ppa_url);

#endif
