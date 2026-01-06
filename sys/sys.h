#ifndef SYS_H
#define SYS_H

#include <stdint.h>

typedef struct
{
    uint32_t uptime;
    uint32_t load_avg[3];
    uint32_t total_ram;
    uint32_t free_ram;
} sys_info_t;

int sys_init(void);
int sys_get_info(sys_info_t *info);
int sys_set_hostname(const char *hostname);
int sys_get_hostname(char *hostname, uint32_t max_len);
int sys_reboot(void);
int sys_shutdown(void);

#endif
