#ifndef COMPATIBILITY_LAYER_H
#define COMPATIBILITY_LAYER_H

#include <stdint.h>
#include <stdbool.h>

typedef enum
{
    COMPAT_WINDOWS,
    COMPAT_LINUX,
    COMPAT_MACOS,
    COMPAT_POSIX,
    COMPAT_BSD
} CompatibilityMode;

typedef enum
{
    API_WIN32,
    API_POSIX,
    API_LINUX,
    API_BSD,
    API_MACOS
} APIType;

typedef struct
{
    CompatibilityMode mode;
    APIType primary_api;
    bool enable_wine_support;
    bool enable_posix_layer;
    char app_name[256];
    char app_path[512];
} CompatibilityConfig;

typedef struct
{
    CompatibilityMode mode;
    CompatibilityConfig config;
    void *wine_instance;
    void *posix_translator;
    bool is_active;
} CompatibilityLayer;

CompatibilityLayer *compat_layer_init(CompatibilityMode mode);
void compat_layer_cleanup(CompatibilityLayer *layer);
int compat_layer_load_app(CompatibilityLayer *layer, const char *app_path);
int compat_layer_translate_syscall(CompatibilityLayer *layer, uint32_t syscall_id, void *args);
int compat_layer_execute_windows_api(CompatibilityLayer *layer, const char *api_name, void *params);
int compat_layer_execute_posix_api(CompatibilityLayer *layer, const char *api_name, void *params);
void compat_layer_enable_wine(CompatibilityLayer *layer, bool enable);

#endif
