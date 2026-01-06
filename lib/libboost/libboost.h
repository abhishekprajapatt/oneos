#ifndef LIBBOOST_H
#define LIBBOOST_H

#include <stdint.h>

typedef struct
{
    uint32_t lib_id;
    const char *lib_name;
    const char *version;
} libboost_info_t;

int libboost_init(void);
int libboost_system_init(void);
int libboost_thread_init(void);
int libboost_filesystem_init(void);
int libboost_get_version(char *version, uint32_t max_len);

#endif
