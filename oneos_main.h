#ifndef ONEOS_MAIN_H
#define ONEOS_MAIN_H

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    char os_name[32];
    uint32_t version_major;
    uint32_t version_minor;
    uint32_t version_patch;
    char kernel_type[64];
    bool is_running;
    uint32_t uptime_seconds;
} OSInfo;

typedef struct
{
    OSInfo info;
    void *kernel;
    void *ui_core;
    void *window_manager;
    void *menu_bar;
    void *dock;
} OneOSSystem;

OneOSSystem *oneos_initialize(void);
void oneos_cleanup(OneOSSystem *system);
int oneos_run(OneOSSystem *system);
void oneos_shutdown(OneOSSystem *system);
void oneos_display_info(OneOSSystem *system);

#endif
