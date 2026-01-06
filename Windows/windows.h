#ifndef WINDOWS_H
#define WINDOWS_H

#include <stdint.h>

int windows_init(void);
int windows_get_version(char *version, uint32_t max_len);
int windows_check_compatibility(void);
int windows_emulation_mode(void);

#endif
