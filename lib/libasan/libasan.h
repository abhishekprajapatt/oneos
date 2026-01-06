#ifndef LIBASAN_H
#define LIBASAN_H

#include <stdint.h>

int libasan_init(void);
int libasan_detect_memory_error(void *addr, uint32_t size);
int libasan_report_error(const char *error_message);
int libasan_enable_instrumentation(void);
int libasan_disable_instrumentation(void);
int libasan_get_status(void);

#endif
