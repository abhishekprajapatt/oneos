#ifndef KCONFIG_PARSER_H
#define KCONFIG_PARSER_H

#include <stdint.h>

typedef struct
{
    const char *option;
    const char *value;
    uint32_t type;
} kconfig_option_t;

int kconfig_init(void);
int kconfig_parse_file(const char *kconfig_path);
int kconfig_get_option(const char *option_name, kconfig_option_t *option);
int kconfig_set_option(const char *option_name, const char *value);
int kconfig_list_options(kconfig_option_t **options, uint32_t *count);
int kconfig_validate(void);
int kconfig_generate_config(const char *output_path);

#endif
