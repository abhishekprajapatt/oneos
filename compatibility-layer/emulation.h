#ifndef EMULATION_H
#define EMULATION_H

#include <stdint.h>

typedef struct
{
    uint32_t emu_mode;
    const char *target_os;
    const char *target_version;
} emulation_config_t;

int emulation_init(void);
int emulation_set_mode(uint32_t mode);
int emulation_load_environment(const char *env_name);
int emulation_translate_syscall(uint32_t syscall);
int emulation_get_mode(void);
int emulation_cleanup(void);

#endif
