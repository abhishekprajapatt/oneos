#include "libc.h"

int libc_init(void)
{
    return 0;
}

int libc_malloc_init(void)
{
    return 0;
}

int libc_stdio_init(void)
{
    return 0;
}

int libc_stdlib_init(void)
{
    return 0;
}

int libc_string_init(void)
{
    return 0;
}

int libc_set_locale(const char *locale)
{
    if (!locale)
    {
        return -1;
    }
    return 0;
}
