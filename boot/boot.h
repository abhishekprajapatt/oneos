#ifndef BOOT_H
#define BOOT_H

#include <stdint.h>

typedef struct
{
    uint32_t memory_size;
    uint32_t cpu_count;
    uint32_t boot_device;
} boot_info_t;

int boot_init(void);
int boot_load_kernel(const char *path);
int boot_get_info(boot_info_t *info);
int boot_early_print(const char *msg);
int boot_setup_framebuffer(void);

#endif
