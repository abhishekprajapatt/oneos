#ifndef ONEOS_SYSTEM_INFO_H
#define ONEOS_SYSTEM_INFO_H

#include <stdint.h>

#define ONEOS_NAME "OneOS"
#define ONEOS_VERSION_MAJOR 1
#define ONEOS_VERSION_MINOR 0
#define ONEOS_VERSION_PATCH 0
#define ONEOS_BUILD_NUMBER 1

#define ONEOS_KERNEL_TYPE "Hybrid-Linux-Unix-XNU"
#define ONEOS_KERNEL_ARCHITECTURE "x86_64"

#define ONEOS_MAX_CPUS 256
#define ONEOS_MAX_MEMORY (16 * 1024 * 1024 * 1024)

#define ONEOS_BOOT_TIMEOUT 60
#define ONEOS_DEFAULT_RUNLEVEL 5

#define ONEOS_DESCRIPTION                             \
    "OneOS - The Ultimate Dream Operating System\n"   \
    "Combining the best of macOS UI elegance,\n"      \
    "Linux/Unix stability, Windows compatibility,\n"  \
    "and BSD reliability.\n\n"                        \
    "Architecture: Hybrid Kernel\n"                   \
    "Base: Linux monolithic + Mach microkernel\n"     \
    "Filesystem: APFS-inspired with Unix hierarchy\n" \
    "UI: macOS-inspired (elegant, minimalist)\n"      \
    "Compatibility: Windows (Wine), Linux, POSIX\n"   \
    "Security: SELinux + macOS sandboxing\n"

typedef struct
{
    const char *os_name;
    uint32_t version_major;
    uint32_t version_minor;
    uint32_t version_patch;
    uint32_t build_number;
    const char *kernel_type;
    const char *kernel_architecture;
    const char *description;
} ONEOSSystemInfo;

const ONEOSSystemInfo *oneos_get_system_info(void);
const char *oneos_get_version_string(void);
const char *oneos_get_description(void);

#endif
