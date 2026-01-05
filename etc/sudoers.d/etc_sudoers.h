#ifndef ETC_SUDOERS_H
#define ETC_SUDOERS_H

#include <stdint.h>

typedef struct
{
    uint32_t rule_id;
    const char *user;
    const char *host;
    const char *command;
    uint32_t flags;
} etc_sudoers_rule_t;

typedef struct
{
    uint32_t rule_count;
    uint32_t user_count;
    uint32_t group_count;
} etc_sudoers_config_t;

int etc_sudoers_init(void);
int etc_sudoers_load_config(const char *filename);
int etc_sudoers_add_rule(etc_sudoers_rule_t *rule);
int etc_sudoers_remove_rule(uint32_t rule_id);
int etc_sudoers_check_permission(const char *user, const char *command);
int etc_sudoers_save_config(const char *filename);

#endif
