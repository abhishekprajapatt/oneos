#ifndef INSTALLER_H
#define INSTALLER_H

#include <stdint.h>

int installer_init(void);
int installer_install_package(const char *package_path);
int installer_uninstall_package(const char *package_name);
int installer_verify_integrity(const char *package_path);
int installer_get_progress(uint32_t *progress);
int installer_cancel_install(void);

#endif
