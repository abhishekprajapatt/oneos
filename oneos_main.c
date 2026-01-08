#include "oneos_main.h"
#include "ui-system/ui_core.h"
#include "ui-system/menu_bar.h"
#include "ui-system/dock.h"
#include "ui-system/window_manager.h"
#include "kernel/kernel_hybrid.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

OneOSSystem *oneos_initialize(void)
{
    OneOSSystem *system = (OneOSSystem *)malloc(sizeof(OneOSSystem));
    if (!system)
        return NULL;

    strcpy(system->info.os_name, "OneOS");
    system->info.version_major = 1;
    system->info.version_minor = 0;
    system->info.version_patch = 0;
    strcpy(system->info.kernel_type, "Hybrid-Linux-Unix-XNU");
    system->info.is_running = false;
    system->info.uptime_seconds = 0;

    system->kernel = kernel_init(KERNEL_HYBRID);
    if (!system->kernel)
    {
        free(system);
        return NULL;
    }

    system->ui_core = ui_core_init();
    if (!system->ui_core)
    {
        kernel_cleanup((KernelCore *)system->kernel);
        free(system);
        return NULL;
    }

    system->window_manager = wm_create(1920, 1080);
    if (!system->window_manager)
    {
        ui_core_cleanup((UICore *)system->ui_core);
        kernel_cleanup((KernelCore *)system->kernel);
        free(system);
        return NULL;
    }

    system->menu_bar = menubar_create();
    if (!system->menu_bar)
    {
        wm_destroy((WindowManager *)system->window_manager);
        ui_core_cleanup((UICore *)system->ui_core);
        kernel_cleanup((KernelCore *)system->kernel);
        free(system);
        return NULL;
    }

    system->dock = dock_create();
    if (!system->dock)
    {
        menubar_destroy((MenuBar *)system->menu_bar);
        wm_destroy((WindowManager *)system->window_manager);
        ui_core_cleanup((UICore *)system->ui_core);
        kernel_cleanup((KernelCore *)system->kernel);
        free(system);
        return NULL;
    }

    MenuBar *mb = (MenuBar *)system->menu_bar;
    menubar_add_item(mb, "OneOS", MENU_ONEOS);
    menubar_add_item(mb, "File", MENU_FILE);
    menubar_add_item(mb, "Edit", MENU_EDIT);
    menubar_add_item(mb, "View", MENU_VIEW);
    menubar_add_item(mb, "Window", MENU_WINDOW);
    menubar_add_item(mb, "Help", MENU_HELP);

    Dock *d = (Dock *)system->dock;
    dock_add_app(d, "Finder", APP_FINDER, "/usr/share/icons/finder.png");
    dock_add_app(d, "System Preferences", APP_SYSTEM_PREFERENCES, "/usr/share/icons/preferences.png");
    dock_add_app(d, "Terminal", APP_TERMINAL, "/usr/share/icons/terminal.png");

    return system;
}

void oneos_cleanup(OneOSSystem *system)
{
    if (!system)
        return;

    if (system->dock)
    {
        dock_destroy((Dock *)system->dock);
    }
    if (system->menu_bar)
    {
        menubar_destroy((MenuBar *)system->menu_bar);
    }
    if (system->window_manager)
    {
        wm_destroy((WindowManager *)system->window_manager);
    }
    if (system->ui_core)
    {
        ui_core_cleanup((UICore *)system->ui_core);
    }
    if (system->kernel)
    {
        kernel_cleanup((KernelCore *)system->kernel);
    }

    free(system);
}

int oneos_run(OneOSSystem *system)
{
    if (!system)
        return -1;

    system->info.is_running = true;
    time_t start_time = time(NULL);

    while (system->info.is_running)
    {
        system->info.uptime_seconds = (uint32_t)(time(NULL) - start_time);

        KernelCore *kernel = (KernelCore *)system->kernel;
        if (kernel)
        {
            kernel_schedule_tasks(kernel);
        }

        UICore *ui = (UICore *)system->ui_core;
        if (ui)
        {
            ui_update_frame(ui);
        }

        usleep(16000);
    }

    return 0;
}

void oneos_shutdown(OneOSSystem *system)
{
    if (!system)
        return;
    system->info.is_running = false;
}

void oneos_display_info(OneOSSystem *system)
{
    if (!system)
        return;

    printf("===========================================\n");
    printf("OneOS - Dream Operating System\n");
    printf("===========================================\n");
    printf("OS Name: %s\n", system->info.os_name);
    printf("Version: %d.%d.%d\n", system->info.version_major,
           system->info.version_minor, system->info.version_patch);
    printf("Kernel: %s\n", system->info.kernel_type);
    printf("Status: %s\n", system->info.is_running ? "Running" : "Stopped");
    printf("Uptime: %u seconds\n", system->info.uptime_seconds);
    printf("===========================================\n");
}
