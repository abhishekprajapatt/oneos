#ifndef LTS_MANAGER_H
#define LTS_MANAGER_H

#include <stdint.h>

int lts_manager_init(void);
int lts_manager_enable_lts_support(const char *version);
int lts_manager_schedule_updates(uint32_t interval);
int lts_manager_apply_security_patches(void);
int lts_manager_get_lts_info(char *info, uint32_t max_len);
int lts_manager_extended_support(uint32_t years);

#endif
