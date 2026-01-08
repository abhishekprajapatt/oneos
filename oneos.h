#ifndef ONEOS_H
#define ONEOS_H

#include <stdint.h>

typedef struct
{
    uint32_t os_id;
    const char *os_name;
    const char *os_version;
    uint32_t build_number;
    const char *kernel_type;
    const char *ui_framework;
} oneos_config_t;

typedef enum
{
    ONEOS_STATE_BOOT,
    ONEOS_STATE_INIT,
    ONEOS_STATE_RUNNING,
    ONEOS_STATE_SHUTDOWN,
    ONEOS_STATE_PANIC
} ONEOSState;

int oneos_init(void);
int oneos_start(void);
int oneos_stop(void);
int oneos_get_version(char *version, uint32_t max_len);
int oneos_get_system_info(void);
int oneos_load_configuration(const char *config_file);
void oneos_boot_sequence(void);
void oneos_kernel_panic(const char *message);
int oneos_main(int argc, char **argv);

#endif
