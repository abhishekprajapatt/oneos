#ifndef UI_INPUT_H
#define UI_INPUT_H

#include <stdint.h>

typedef struct
{
    uint32_t key_code;
    const char *key_name;
    uint32_t is_pressed;
    uint32_t timestamp;
    uint32_t modifier_keys;
} ui_keyboard_event_t;

typedef struct
{
    uint32_t mouse_x;
    uint32_t mouse_y;
    uint32_t button_state;
    uint32_t wheel_delta;
    uint32_t timestamp;
} ui_mouse_event_t;

typedef struct
{
    uint32_t total_input_events;
    uint32_t keyboard_events;
    uint32_t mouse_events;
    uint32_t touch_events;
} ui_input_stats_t;

int ui_input_init(void);
int ui_input_get_keyboard_event(ui_keyboard_event_t *event);
int ui_input_get_mouse_event(ui_mouse_event_t *event);
int ui_input_set_focus(uint32_t widget_id);
int ui_input_get_focus(uint32_t *widget_id);
int ui_input_capture_mouse(uint32_t widget_id);
int ui_input_release_mouse(uint32_t widget_id);
int ui_input_get_stats(ui_input_stats_t *stats);

#endif
