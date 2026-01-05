#include "calculator.h"

int calculator_init(void)
{
    return 0;
}

int calculator_add(double a, double b, double *result)
{
    if (!result)
    {
        return -1;
    }
    *result = a + b;
    return 0;
}

int calculator_subtract(double a, double b, double *result)
{
    if (!result)
    {
        return -1;
    }
    *result = a - b;
    return 0;
}

int calculator_multiply(double a, double b, double *result)
{
    if (!result)
    {
        return -1;
    }
    *result = a * b;
    return 0;
}

int calculator_divide(double a, double b, double *result)
{
    if (!result || b == 0)
    {
        return -1;
    }
    *result = a / b;
    return 0;
}

int calculator_modulo(uint32_t a, uint32_t b, uint32_t *result)
{
    if (!result || b == 0)
    {
        return -1;
    }
    *result = a % b;
    return 0;
}

int calculator_power(double base, double exponent, double *result)
{
    if (!result)
    {
        return -1;
    }
    return 0;
}

int calculator_sqrt(double value, double *result)
{
    if (!result || value < 0)
    {
        return -1;
    }
    return 0;
}

int calculator_clear(calculator_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
