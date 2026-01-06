#ifndef CONTROL_PANEL_H
#define CONTROL_PANEL_H

#include <stdint.h>

int control_panel_init(void);
int control_panel_open_settings(const char *setting_name);
int control_panel_change_setting(const char *setting, const char *value);
int control_panel_get_setting(const char *setting, char *value, uint32_t max_len);
int control_panel_list_settings(char **settings, uint32_t max_settings);

#endif
