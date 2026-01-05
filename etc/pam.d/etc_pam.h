#ifndef ETC_PAM_H
#define ETC_PAM_H

#include <stdint.h>

typedef struct
{
    uint32_t rule_id;
    const char *service;
    const char *module_path;
    uint32_t control_flag;
} etc_pam_rule_t;

typedef struct
{
    uint32_t rule_count;
    uint32_t module_count;
    uint32_t version;
} etc_pam_config_t;

int etc_pam_init(void);
int etc_pam_load_config(const char *filename);
int etc_pam_add_rule(etc_pam_rule_t *rule);
int etc_pam_remove_rule(uint32_t rule_id);
int etc_pam_authenticate(const char *service, const char *username, const char *password);
int etc_pam_check_rule(const char *service);

#endif
