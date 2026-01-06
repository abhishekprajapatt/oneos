#ifndef LIBCAP_H
#define LIBCAP_H

#include <stdint.h>

typedef struct
{
    uint64_t capabilities;
    uint32_t effective;
    uint32_t permitted;
    uint32_t inheritable;
} libcap_t;

int libcap_init(void);
int libcap_get_current(libcap_t *cap);
int libcap_set_current(libcap_t *cap);
int libcap_drop_capability(uint32_t capability);
int libcap_clear(void);

#endif
