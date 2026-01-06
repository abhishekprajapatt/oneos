#ifndef BETA_MANAGER_H
#define BETA_MANAGER_H

#include <stdint.h>

typedef struct
{
    const char *beta_version;
    uint32_t tester_count;
    uint32_t feedback_count;
} beta_info_t;

int beta_manager_init(void);
int beta_manager_create_beta_build(const char *base_version);
int beta_manager_add_tester(const char *tester_email);
int beta_manager_collect_feedback(const char *tester_id, const char *feedback);
int beta_manager_generate_beta_report(const char *output_file);
int beta_manager_promote_to_release(const char *beta_version);

#endif
