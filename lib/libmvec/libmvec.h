#ifndef LIBMVEC_H
#define LIBMVEC_H

#include <stdint.h>

int libmvec_init(void);
int libmvec_sinf(float *x, uint32_t count);
int libmvec_cosf(float *x, uint32_t count);
int libmvec_expf(float *x, uint32_t count);
int libmvec_logf(float *x, uint32_t count);
int libmvec_set_mode(uint32_t mode);

#endif
