#ifndef LIBRT_STATIC_H
#define LIBRT_STATIC_H

#include <stdint.h>

typedef void timer_t;

int librt_static_init(void);
int librt_static_timer_create(clockid_t clockid, void *sevp, timer_t **timerid);
int librt_static_clock_gettime(uint32_t clockid, void *tp);

#endif
