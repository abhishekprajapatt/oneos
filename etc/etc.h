#ifndef ETC_H
#define ETC_H

#include <stdint.h>

int etc_init(void);
int etc_load_config(const char *config_name, void **config);
int etc_save_config(const char *config_name, void *config);
int etc_get_setting(const char *setting, char *value, uint32_t max_len);
int etc_set_setting(const char *setting, const char *value);

#endif
