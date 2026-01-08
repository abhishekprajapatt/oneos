#include "display_server.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

DisplayServer *display_server_init(uint32_t width, uint32_t height)
{
    DisplayServer *server = (DisplayServer *)malloc(sizeof(DisplayServer));
    if (!server)
        return NULL;

    server->config.width = width;
    server->config.height = height;
    server->config.bits_per_pixel = 32;
    server->config.refresh_rate = 60;
    snprintf(server->config.resolution, 32, "%ux%u", width, height);

    server->framebuffer_size = width * height * (server->config.bits_per_pixel / 8);
    server->framebuffer = malloc(server->framebuffer_size);

    if (!server->framebuffer)
    {
        free(server);
        return NULL;
    }

    memset(server->framebuffer, 0, server->framebuffer_size);
    server->is_active = true;

    return server;
}

void display_server_destroy(DisplayServer *server)
{
    if (!server)
        return;
    if (server->framebuffer)
    {
        free(server->framebuffer);
    }
    free(server);
}

void display_server_update(DisplayServer *server)
{
    if (!server || !server->is_active)
        return;
}

void display_server_set_resolution(DisplayServer *server, uint32_t width, uint32_t height)
{
    if (!server)
        return;

    server->config.width = width;
    server->config.height = height;
    snprintf(server->config.resolution, 32, "%ux%u", width, height);

    uint32_t new_size = width * height * (server->config.bits_per_pixel / 8);
    void *new_framebuffer = realloc(server->framebuffer, new_size);

    if (new_framebuffer)
    {
        server->framebuffer = new_framebuffer;
        server->framebuffer_size = new_size;
    }
}

void display_server_set_refresh_rate(DisplayServer *server, uint32_t rate)
{
    if (!server)
        return;
    server->config.refresh_rate = rate;
}

int display_server_get_framebuffer(DisplayServer *server, void **buffer)
{
    if (!server || !buffer)
        return -1;
    *buffer = server->framebuffer;
    return server->framebuffer_size;
}
