#ifndef DISPLAY_SERVER_H
#define DISPLAY_SERVER_H

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    uint32_t width;
    uint32_t height;
    uint32_t bits_per_pixel;
    uint32_t refresh_rate;
    char resolution[32];
} DisplayConfig;

typedef struct
{
    DisplayConfig config;
    uint32_t framebuffer_size;
    void *framebuffer;
    bool is_active;
} DisplayServer;

DisplayServer *display_server_init(uint32_t width, uint32_t height);
void display_server_destroy(DisplayServer *server);
void display_server_update(DisplayServer *server);
void display_server_set_resolution(DisplayServer *server, uint32_t width, uint32_t height);
void display_server_set_refresh_rate(DisplayServer *server, uint32_t rate);
int display_server_get_framebuffer(DisplayServer *server, void **buffer);

#endif
