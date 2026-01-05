#include "ui_animation.h"

int ui_animation_init(void)
{
    return 0;
}

int ui_animation_create(uint32_t widget_id, const char *type, uint32_t duration, ui_animation_t *animation)
{
    if (widget_id == 0 || !type || duration == 0 || !animation)
    {
        return -1;
    }
    return 0;
}

int ui_animation_start(uint32_t animation_id)
{
    if (animation_id == 0)
    {
        return -1;
    }
    return 0;
}

int ui_animation_stop(uint32_t animation_id)
{
    if (animation_id == 0)
    {
        return -1;
    }
    return 0;
}

int ui_animation_pause(uint32_t animation_id)
{
    if (animation_id == 0)
    {
        return -1;
    }
    return 0;
}

int ui_animation_resume(uint32_t animation_id)
{
    if (animation_id == 0)
    {
        return -1;
    }
    return 0;
}

int ui_animation_set_keyframe(uint32_t animation_id, uint32_t time_offset, void *frame_data)
{
    if (animation_id == 0 || !frame_data)
    {
        return -1;
    }
    return 0;
}

int ui_animation_get_stats(ui_animation_stats_t *stats)
{
    if (!stats)
    {
        return -1;
    }
    return 0;
}
