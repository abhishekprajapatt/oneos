#ifndef APPLICATION_SUPPORT_H
#define APPLICATION_SUPPORT_H

#include <stdint.h>

int app_support_init(void);
int app_support_get_path(const char *app_name, char *path, uint32_t max_len);
int app_support_create_directory(const char *app_name, const char *dir);
int app_support_save_data(const char *app_name, const char *data_name, const void *data, uint32_t size);
int app_support_load_data(const char *app_name, const char *data_name, void *buffer, uint32_t max_size);
int app_support_list_apps(char **apps, uint32_t max_apps);

#endif
