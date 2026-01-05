#ifndef TOAST_H
#define TOAST_H

#include <stdint.h>

typedef struct
{
    uint32_t toast_id;
    const char *message;
    const char *toast_type;
    uint32_t duration_ms;
    const char *position;
    uint32_t show_icon;
} toast_config_t;

typedef struct
{
    uint32_t toast_id;
    uint32_t is_visible;
    uint32_t is_showing;
    uint64_t show_time;
} toast_state_t;

typedef struct
{
    uint32_t total_toasts;
    uint32_t displayed_toasts;
    uint32_t dismissed_toasts;
    uint64_t total_display_time;
} toast_stats_t;

int toast_init(void);
int toast_create(toast_config_t *config, uint32_t *toast_id);
int toast_show(uint32_t toast_id);
int toast_dismiss(uint32_t toast_id);
int toast_update_message(uint32_t toast_id, const char *new_message);
int toast_set_duration(uint32_t toast_id, uint32_t duration_ms);
int toast_get_state(uint32_t toast_id, toast_state_t *state);
int toast_get_stats(toast_stats_t *stats);
int toast_cleanup(void);

#endif
