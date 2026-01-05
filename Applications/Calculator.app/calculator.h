#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <stdint.h>

typedef struct
{
    double result;
    double operand1;
    double operand2;
    const char *operation;
    uint32_t decimal_places;
} calculator_state_t;

int calculator_init(void);
int calculator_add(double a, double b, double *result);
int calculator_subtract(double a, double b, double *result);
int calculator_multiply(double a, double b, double *result);
int calculator_divide(double a, double b, double *result);
int calculator_modulo(uint32_t a, uint32_t b, uint32_t *result);
int calculator_power(double base, double exponent, double *result);
int calculator_sqrt(double value, double *result);
int calculator_clear(calculator_state_t *state);

#endif
