#ifndef OPT_H
#define OPT_H

#include <stdint.h>

int opt_init(void);
int opt_install_package(const char *package);
int opt_uninstall_package(const char *package);
int opt_get_installed(char **packages, uint32_t max_packages);
int opt_check_integrity(const char *package);

#endif
