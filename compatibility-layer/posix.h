#ifndef POSIX_H
#define POSIX_H

#include <stdint.h>

typedef struct
{
    uint32_t feature_id;
    const char *feature_name;
    uint32_t supported;
} posix_feature_t;

int posix_init(void);
int posix_check_feature(const char *feature_name);
int posix_map_syscall(uint32_t syscall_number);
int posix_get_version(char *version, uint32_t max_len);
int posix_enable_extension(const char *extension_name);
int posix_list_features(posix_feature_t *features, uint32_t *count);

#endif
