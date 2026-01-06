#ifndef APP_INSTALLER_H
#define APP_INSTALLER_H

#include <stdint.h>

int app_installer_init(void);
int app_installer_extract_msi(const char *msi_file, const char *dest_path);
int app_installer_install_app(const char *app_path);
int app_installer_configure_shortcuts(const char *app_name);
int app_installer_register_file_types(const char *app_name, const char **extensions, uint32_t count);
int app_installer_validate_installation(const char *app_name);

#endif
