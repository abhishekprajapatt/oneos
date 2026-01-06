#ifndef ROOT_H
#define ROOT_H

#include <stdint.h>

int root_init(void);
int root_create_file(const char *path);
int root_delete_file(const char *path);
int root_create_directory(const char *path);
int root_delete_directory(const char *path);
int root_get_current_dir(char *path, uint32_t max_len);

#endif
