#ifndef API_WRAPPER_H
#define API_WRAPPER_H

#include <stdint.h>

typedef struct
{
    uint32_t wrapper_id;
    const char *api_name;
    const char *target_version;
} api_wrapper_config_t;

int api_wrapper_init(void);
int api_wrapper_register(api_wrapper_config_t *config);
int api_wrapper_call(const char *api_name, void *args);
int api_wrapper_translate_error(int error_code);
int api_wrapper_get_version_mapping(const char *api_name, char *version, uint32_t max_len);
int api_wrapper_validate_compatibility(const char *api_name);

#endif
