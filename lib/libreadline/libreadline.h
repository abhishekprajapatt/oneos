#ifndef LIBREADLINE_H
#define LIBREADLINE_H

int libreadline_init(void);
int libreadline_read_history(const char *filename);
int libreadline_write_history(const char *filename);
int libreadline_add_history(const char *line);
int libreadline_clear_history(void);
char *libreadline_readline(const char *prompt);

#endif
