#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <stdint.h>
#include <stdbool.h>

typedef enum
{
    INPUT_MOUSE,
    INPUT_KEYBOARD,
    INPUT_TOUCHPAD,
    INPUT_JOYSTICK
} InputType;

typedef enum
{
    KEY_PRESS,
    KEY_RELEASE
} KeyEvent;

typedef enum
{
    MOUSE_MOVE,
    MOUSE_CLICK,
    MOUSE_SCROLL
} MouseEvent;

typedef struct
{
    uint32_t x;
    uint32_t y;
    bool left_button;
    bool right_button;
    bool middle_button;
} MouseState;

typedef struct
{
    uint32_t key_code;
    KeyEvent event;
    bool shift;
    bool ctrl;
    bool alt;
    bool cmd;
} KeyboardState;

typedef struct
{
    InputType type;
    union
    {
        MouseState mouse;
        KeyboardState keyboard;
    } state;
} InputEvent;

typedef struct
{
    InputEvent *events;
    uint32_t event_count;
    uint32_t max_events;
    MouseState mouse_state;
} InputManager;

InputManager *input_manager_create(void);
void input_manager_destroy(InputManager *manager);
void input_manager_add_event(InputManager *manager, InputEvent event);
int input_manager_get_events(InputManager *manager, InputEvent **events);
void input_manager_clear_events(InputManager *manager);
MouseState input_manager_get_mouse_state(InputManager *manager);

#endif
