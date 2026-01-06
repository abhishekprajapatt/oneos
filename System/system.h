#ifndef SYSTEM_H
#define SYSTEM_H

#include <stdint.h>

int system_init(void);
int system_get_info(char *info, uint32_t max_len);
int system_check_health(uint32_t *health_status);
int system_update(const char *update_path);
int system_reset(void);
int system_get_version(char *version, uint32_t max_len);

#endif
