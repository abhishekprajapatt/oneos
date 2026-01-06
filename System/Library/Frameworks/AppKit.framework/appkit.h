#ifndef APPKIT_H
#define APPKIT_H

#include <stdint.h>

typedef void NSApplication;
typedef void NSWindow;

int appkit_init(void);
int appkit_create_application(NSApplication **app);
int appkit_create_window(uint32_t width, uint32_t height, NSWindow **window);
int appkit_run_event_loop(void);
int appkit_shutdown(void);
int appkit_set_menu(const char *menu_items);

#endif
