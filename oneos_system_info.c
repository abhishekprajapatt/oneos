#include "oneos_system_info.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static ONEOSSystemInfo g_system_info = {
    .os_name = ONEOS_NAME,
    .version_major = ONEOS_VERSION_MAJOR,
    .version_minor = ONEOS_VERSION_MINOR,
    .version_patch = ONEOS_VERSION_PATCH,
    .build_number = ONEOS_BUILD_NUMBER,
    .kernel_type = ONEOS_KERNEL_TYPE,
    .kernel_architecture = ONEOS_KERNEL_ARCHITECTURE,
    .description = ONEOS_DESCRIPTION};

const ONEOSSystemInfo *oneos_get_system_info(void)
{
    return &g_system_info;
}

const char *oneos_get_version_string(void)
{
    static char version_string[64];
    snprintf(version_string, sizeof(version_string), "%d.%d.%d (Build %d)",
             g_system_info.version_major,
             g_system_info.version_minor,
             g_system_info.version_patch,
             g_system_info.build_number);
    return version_string;
}

const char *oneos_get_description(void)
{
    return g_system_info.description;
}
