#include "libreadline.h"

int libreadline_init(void)
{
    return 0;
}

int libreadline_read_history(const char *filename)
{
    if (!filename)
    {
        return -1;
    }
    return 0;
}

int libreadline_write_history(const char *filename)
{
    if (!filename)
    {
        return -1;
    }
    return 0;
}

int libreadline_add_history(const char *line)
{
    if (!line)
    {
        return -1;
    }
    return 0;
}

int libreadline_clear_history(void)
{
    return 0;
}

char *libreadline_readline(const char *prompt)
{
    if (!prompt)
    {
        return 0;
    }
    return 0;
}
