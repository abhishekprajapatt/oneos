#ifndef LIBNCURSES_H
#define LIBNCURSES_H

#include <stdint.h>

int libncurses_init(void);
int libncurses_initscr(void);
int libncurses_endwin(void);
int libncurses_addstr(const char *str);
int libncurses_mvaddstr(uint32_t y, uint32_t x, const char *str);
int libncurses_refresh(void);
int libncurses_getch(int *ch);

#endif
