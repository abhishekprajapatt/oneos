#include "compatibility_layer.h"
#include <stdlib.h>
#include <string.h>

CompatibilityLayer *compat_layer_init(CompatibilityMode mode)
{
    CompatibilityLayer *layer = (CompatibilityLayer *)malloc(sizeof(CompatibilityLayer));
    if (!layer)
        return NULL;

    layer->mode = mode;
    layer->is_active = true;
    layer->wine_instance = NULL;
    layer->posix_translator = NULL;

    switch (mode)
    {
    case COMPAT_WINDOWS:
        layer->config.primary_api = API_WIN32;
        layer->config.enable_wine_support = true;
        break;
    case COMPAT_LINUX:
    case COMPAT_POSIX:
    case COMPAT_BSD:
        layer->config.primary_api = API_POSIX;
        layer->config.enable_posix_layer = true;
        break;
    case COMPAT_MACOS:
        layer->config.primary_api = API_MACOS;
        break;
    default:
        free(layer);
        return NULL;
    }

    strcpy(layer->config.app_name, "OneOS Application");
    strcpy(layer->config.app_path, "/usr/bin/");

    return layer;
}

void compat_layer_cleanup(CompatibilityLayer *layer)
{
    if (!layer)
        return;
    if (layer->wine_instance)
    {
        free(layer->wine_instance);
    }
    if (layer->posix_translator)
    {
        free(layer->posix_translator);
    }
    free(layer);
}

int compat_layer_load_app(CompatibilityLayer *layer, const char *app_path)
{
    if (!layer || !app_path)
        return -1;

    strncpy(layer->config.app_path, app_path, 511);

    return 0;
}

int compat_layer_translate_syscall(CompatibilityLayer *layer, uint32_t syscall_id, void *args)
{
    if (!layer)
        return -1;

    switch (layer->mode)
    {
    case COMPAT_WINDOWS:
        return 0;
    case COMPAT_POSIX:
    case COMPAT_LINUX:
    case COMPAT_BSD:
        return 0;
    default:
        return -1;
    }
}

int compat_layer_execute_windows_api(CompatibilityLayer *layer, const char *api_name, void *params)
{
    if (!layer || !api_name)
        return -1;

    if (layer->mode != COMPAT_WINDOWS && !layer->config.enable_wine_support)
    {
        return -1;
    }

    return 0;
}

int compat_layer_execute_posix_api(CompatibilityLayer *layer, const char *api_name, void *params)
{
    if (!layer || !api_name)
        return -1;

    if (layer->mode == COMPAT_WINDOWS && !layer->config.enable_posix_layer)
    {
        return -1;
    }

    return 0;
}

void compat_layer_enable_wine(CompatibilityLayer *layer, bool enable)
{
    if (!layer)
        return;
    layer->config.enable_wine_support = enable;
}
