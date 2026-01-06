#ifndef LIBRT_H
#define LIBRT_H

#include <stdint.h>

typedef void timer_t;

int librt_init(void);
int librt_timer_create(clockid_t clockid, void *sevp, timer_t **timerid);
int librt_timer_delete(timer_t *timerid);
int librt_timer_settime(timer_t *timerid, int flags, const void *new_value, void *old_value);
int librt_clock_gettime(uint32_t clockid, void *tp);

#endif
