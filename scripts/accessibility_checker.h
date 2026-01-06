#ifndef ACCESSIBILITY_CHECKER_H
#define ACCESSIBILITY_CHECKER_H

#include <stdint.h>

typedef struct
{
    const char *issue;
    const char *location;
    uint32_t severity;
} accessibility_issue_t;

int accessibility_checker_init(void);
int accessibility_checker_scan(const char *target_path);
int accessibility_checker_check_wcag(uint32_t wcag_level);
int accessibility_checker_get_issues(accessibility_issue_t **issues, uint32_t *count);
int accessibility_checker_generate_report(const char *output_file);
int accessibility_checker_validate_compliance(void);

#endif
