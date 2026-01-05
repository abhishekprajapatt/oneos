#ifndef ALERT_H
#define ALERT_H

#include <stdint.h>

typedef struct
{
    uint32_t alert_id;
    const char *title;
    const char *message;
    const char *alert_type;
    uint32_t priority;
    uint32_t auto_dismiss;
    uint32_t duration_ms;
} alert_config_t;

typedef struct
{
    uint32_t alert_id;
    uint32_t is_visible;
    uint32_t is_active;
    uint64_t creation_time;
} alert_state_t;

typedef struct
{
    uint32_t total_alerts;
    uint32_t active_alerts;
    uint32_t dismissed_alerts;
    uint64_t total_display_time;
} alert_stats_t;

int alert_init(void);
int alert_create(alert_config_t *config, uint32_t *alert_id);
int alert_show(uint32_t alert_id);
int alert_dismiss(uint32_t alert_id);
int alert_update_message(uint32_t alert_id, const char *new_message);
int alert_set_priority(uint32_t alert_id, uint32_t priority);
int alert_get_state(uint32_t alert_id, alert_state_t *state);
int alert_get_stats(alert_stats_t *stats);
int alert_cleanup(void);

#endif
