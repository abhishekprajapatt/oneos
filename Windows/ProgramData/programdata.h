#ifndef PROGRAM_DATA_H
#define PROGRAM_DATA_H

#include <stdint.h>

int programdata_init(void);
int programdata_create_app_folder(const char *app_name);
int programdata_write_config(const char *app_name, const char *config_data);
int programdata_read_config(const char *app_name, char *config_data, uint32_t max_len);
int programdata_delete_app_data(const char *app_name);
int programdata_get_app_size(const char *app_name, uint64_t *size);
int programdata_list_apps(char **apps, uint32_t max_apps);

#endif
