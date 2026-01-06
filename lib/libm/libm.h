#ifndef LIBM_H
#define LIBM_H

#include <stdint.h>

double libm_sin(double x);
double libm_cos(double x);
double libm_tan(double x);
double libm_sqrt(double x);
double libm_pow(double x, double y);
int libm_init(void);

#endif
