#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>

typedef struct
{
    uint32_t version;
    uint32_t build_number;
} kernel_version_t;

int kernel_init(void);
int kernel_start(void);
int kernel_shutdown(void);
int kernel_get_version(kernel_version_t *version);
int kernel_get_uptime(uint64_t *uptime);
int kernel_panic(const char *msg);

#endif
