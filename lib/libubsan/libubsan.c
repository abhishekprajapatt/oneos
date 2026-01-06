#include "libubsan.h"

int libubsan_init(void)
{
    return 0;
}

int libubsan_report_overflow(void *loc, uint32_t kind)
{
    if (!loc)
    {
        return -1;
    }
    return 0;
}

int libubsan_report_out_of_bounds(void *index, void *array)
{
    if (!index || !array)
    {
        return -1;
    }
    return 0;
}

int libubsan_report_invalid_builtin(uint32_t kind)
{
    if (kind == 0)
    {
        return -1;
    }
    return 0;
}

int libubsan_report_float_cast_overflow(void *loc)
{
    if (!loc)
    {
        return -1;
    }
    return 0;
}
