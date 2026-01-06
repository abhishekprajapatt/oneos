#include "libncurses.h"

int libncurses_init(void)
{
    return 0;
}

int libncurses_initscr(void)
{
    return 0;
}

int libncurses_endwin(void)
{
    return 0;
}

int libncurses_addstr(const char *str)
{
    if (!str)
    {
        return -1;
    }
    return 0;
}

int libncurses_mvaddstr(uint32_t y, uint32_t x, const char *str)
{
    if (!str)
    {
        return -1;
    }
    return 0;
}

int libncurses_refresh(void)
{
    return 0;
}

int libncurses_getch(int *ch)
{
    if (!ch)
    {
        return -1;
    }
    return 0;
}
