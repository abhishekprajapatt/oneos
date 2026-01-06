#ifndef PROGRAMFILES_X86_H
#define PROGRAMFILES_X86_H

#include <stdint.h>

int programfiles_x86_init(void);
int programfiles_x86_register_app(const char *app_name, const char *app_path);
int programfiles_x86_uninstall_app(const char *app_name);
int programfiles_x86_list_installed(char **apps, uint32_t max_apps);
int programfiles_x86_get_app_info(const char *app_name, char *info, uint32_t max_len);
int programfiles_x86_verify_compatibility(const char *app_name);

#endif
