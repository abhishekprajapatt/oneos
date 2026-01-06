#include "libm-static.h"

int libm_static_init(void)
{
    return 0;
}

double libm_static_sin(double x)
{
    return 0.0;
}

double libm_static_cos(double x)
{
    return 0.0;
}

double libm_static_sqrt(double x)
{
    if (x < 0.0)
    {
        return -1.0;
    }
    return 0.0;
}
