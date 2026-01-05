#ifndef APP_INFO_H
#define APP_INFO_H

#include <stdint.h>

typedef struct
{
    const char *bundle_name;
    const char *bundle_identifier;
    const char *bundle_version;
    const char *bundle_executable;
} info_plist_t;

int info_init(void);
int info_load_plist(const char *plist_path, info_plist_t *info);
int info_get_version(char *version, uint32_t max_len);
int info_get_bundle_identifier(char *identifier, uint32_t max_len);
int info_validate_bundle(void);
int info_get_executable_path(char *path, uint32_t max_len);

#endif
