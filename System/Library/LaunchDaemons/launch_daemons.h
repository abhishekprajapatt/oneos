#ifndef LAUNCH_DAEMONS_H
#define LAUNCH_DAEMONS_H

#include <stdint.h>

typedef struct
{
    const char *label;
    const char *program;
    uint32_t pid;
    uint32_t status;
} launch_daemon_t;

int launch_daemons_init(void);
int launch_daemons_load(const char *plist_path);
int launch_daemons_unload(const char *daemon_label);
int launch_daemons_start(const char *daemon_label);
int launch_daemons_stop(const char *daemon_label);
int launch_daemons_list(launch_daemon_t **daemons, uint32_t *count);
int launch_daemons_get_status(const char *daemon_label, uint32_t *status);

#endif
