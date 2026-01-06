#ifndef LIBRARY_H
#define LIBRARY_H

#include <stdint.h>

int library_init(void);
int library_load_plugin(const char *plugin_path);
int library_unload_plugin(const char *plugin_name);
int library_get_symbol(const char *plugin_name, const char *symbol, void **ptr);
int library_list_plugins(char **plugins, uint32_t max_plugins);

#endif
