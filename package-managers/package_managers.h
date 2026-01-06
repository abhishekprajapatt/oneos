#ifndef PACKAGE_MANAGERS_H
#define PACKAGE_MANAGERS_H

#include <stdint.h>

int package_managers_init(void);
int package_managers_register(const char *manager_name, void (*handler)(void));
int package_managers_install(const char *manager, const char *package);
int package_managers_uninstall(const char *manager, const char *package);
int package_managers_update(const char *manager);
int package_managers_list_packages(const char *manager, char **packages, uint32_t max);

#endif
