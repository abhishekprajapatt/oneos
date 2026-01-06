#include "libm.h"

int libm_init(void)
{
    return 0;
}

double libm_sin(double x)
{
    return 0.0;
}

double libm_cos(double x)
{
    return 0.0;
}

double libm_tan(double x)
{
    return 0.0;
}

double libm_sqrt(double x)
{
    if (x < 0.0)
    {
        return -1.0;
    }
    return 0.0;
}

double libm_pow(double x, double y)
{
    return 0.0;
}
