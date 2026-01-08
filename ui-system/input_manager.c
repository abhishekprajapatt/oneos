#include "input_manager.h"
#include <stdlib.h>
#include <string.h>

InputManager *input_manager_create(void)
{
    InputManager *manager = (InputManager *)malloc(sizeof(InputManager));
    if (!manager)
        return NULL;

    manager->max_events = 1024;
    manager->event_count = 0;
    manager->events = (InputEvent *)malloc(manager->max_events * sizeof(InputEvent));

    if (!manager->events)
    {
        free(manager);
        return NULL;
    }

    manager->mouse_state.x = 0;
    manager->mouse_state.y = 0;
    manager->mouse_state.left_button = false;
    manager->mouse_state.right_button = false;
    manager->mouse_state.middle_button = false;

    return manager;
}

void input_manager_destroy(InputManager *manager)
{
    if (!manager)
        return;
    if (manager->events)
    {
        free(manager->events);
    }
    free(manager);
}

void input_manager_add_event(InputManager *manager, InputEvent event)
{
    if (!manager || manager->event_count >= manager->max_events)
        return;

    manager->events[manager->event_count] = event;
    manager->event_count++;

    if (event.type == INPUT_MOUSE)
    {
        manager->mouse_state = event.state.mouse;
    }
}

int input_manager_get_events(InputManager *manager, InputEvent **events)
{
    if (!manager || !events)
        return 0;
    *events = manager->events;
    return manager->event_count;
}

void input_manager_clear_events(InputManager *manager)
{
    if (!manager)
        return;
    manager->event_count = 0;
}

MouseState input_manager_get_mouse_state(InputManager *manager)
{
    MouseState empty = {0};
    if (!manager)
        return empty;
    return manager->mouse_state;
}
