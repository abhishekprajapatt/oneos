#ifndef LIBUBSAN_H
#define LIBUBSAN_H

#include <stdint.h>

int libubsan_init(void);
int libubsan_report_overflow(void *loc, uint32_t kind);
int libubsan_report_out_of_bounds(void *index, void *array);
int libubsan_report_invalid_builtin(uint32_t kind);
int libubsan_report_float_cast_overflow(void *loc);

#endif
