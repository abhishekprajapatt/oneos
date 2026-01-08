#include "oneos.h"
#include "oneos_main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static OneOSSystem *g_system = NULL;
static ONEOSState g_state = ONEOS_STATE_BOOT;

int oneos_init(void)
{
    g_system = oneos_initialize();
    if (!g_system)
    {
        return -1;
    }
    g_state = ONEOS_STATE_INIT;
    return 0;
}

int oneos_start(void)
{
    if (!g_system)
    {
        return -1;
    }
    g_state = ONEOS_STATE_RUNNING;
    return oneos_run(g_system);
}

int oneos_stop(void)
{
    if (!g_system)
    {
        return -1;
    }
    g_state = ONEOS_STATE_SHUTDOWN;
    oneos_shutdown(g_system);
    oneos_cleanup(g_system);
    g_system = NULL;
    return 0;
}

int oneos_get_version(char *version, uint32_t max_len)
{
    if (!version || max_len == 0)
    {
        return -1;
    }
    snprintf(version, max_len, "1.0.0");
    return 0;
}

int oneos_get_system_info(void)
{
    if (!g_system)
    {
        return -1;
    }
    oneos_display_info(g_system);
    return 0;
}

int oneos_load_configuration(const char *config_file)
{
    if (!config_file)
    {
        return -1;
    }
    return 0;
}

void oneos_boot_sequence(void)
{
    printf("OneOS Boot Sequence Starting...\n");
    printf("Initializing kernel...\n");
    if (oneos_init() != 0)
    {
        printf("Failed to initialize OneOS\n");
        return;
    }
    printf("Kernel initialized successfully\n");
    printf("Starting OneOS...\n");
}

void oneos_kernel_panic(const char *message)
{
    if (!message)
    {
        message = "Unknown panic";
    }
    g_state = ONEOS_STATE_PANIC;
    printf("\n=== KERNEL PANIC ===\n");
    printf("Error: %s\n", message);
    printf("System halted\n");
    oneos_cleanup(g_system);
    exit(1);
}

int oneos_main(int argc, char **argv)
{
    oneos_boot_sequence();
    if (g_state != ONEOS_STATE_INIT)
    {
        return 1;
    }
    return oneos_start();
}
