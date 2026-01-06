#ifndef WIN32_COMPAT_H
#define WIN32_COMPAT_H

#include <stdint.h>

int win32_compat_init(void);
int win32_compat_load_library(const char *lib_name);
int win32_compat_get_proc_address(const char *lib_name, const char *func_name);
int win32_compat_call_function(const char *func_pointer, void *args);
int win32_compat_unload_library(const char *lib_name);
int win32_compat_translate_error(uint32_t error_code);

#endif
