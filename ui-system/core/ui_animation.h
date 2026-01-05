#ifndef UI_ANIMATION_H
#define UI_ANIMATION_H

#include <stdint.h>

typedef struct
{
    uint32_t animation_id;
    uint32_t target_widget_id;
    const char *animation_type;
    uint32_t duration;
    const char *easing_function;
    uint32_t is_running;
} ui_animation_t;

typedef struct
{
    uint32_t total_animations;
    uint32_t running_animations;
    uint32_t completed_animations;
    uint64_t total_animation_time;
} ui_animation_stats_t;

int ui_animation_init(void);
int ui_animation_create(uint32_t widget_id, const char *type, uint32_t duration, ui_animation_t *animation);
int ui_animation_start(uint32_t animation_id);
int ui_animation_stop(uint32_t animation_id);
int ui_animation_pause(uint32_t animation_id);
int ui_animation_resume(uint32_t animation_id);
int ui_animation_set_keyframe(uint32_t animation_id, uint32_t time_offset, void *frame_data);
int ui_animation_get_stats(ui_animation_stats_t *stats);

#endif
