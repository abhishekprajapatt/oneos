#include "graphics_core.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

GraphicsCore *graphics_init(void)
{
    GraphicsCore *core = (GraphicsCore *)malloc(sizeof(GraphicsCore));
    if (!core)
        return NULL;

    memset(core, 0, sizeof(GraphicsCore));
    core->fb_count = 0;
    core->surface_count = 0;
    core->active_fb_id = 0;
    core->clear_color = (Color){0, 0, 0, 255};
    core->total_pixels_drawn = 0;
    core->total_operations = 0;

    return core;
}

void graphics_cleanup(GraphicsCore *core)
{
    if (!core)
        return;

    for (uint32_t i = 0; i < core->fb_count; i++)
    {
        if (core->frame_buffers[i].pixels)
        {
            free(core->frame_buffers[i].pixels);
        }
    }

    for (uint32_t i = 0; i < core->surface_count; i++)
    {
        if (core->surfaces[i].pixels)
        {
            free(core->surfaces[i].pixels);
        }
    }

    memset(core, 0, sizeof(GraphicsCore));
    free(core);
}

uint32_t graphics_create_framebuffer(GraphicsCore *core, uint32_t width,
                                     uint32_t height, PixelFormat format)
{
    if (!core || core->fb_count >= MAX_FRAME_BUFFERS)
        return 0;
    if (width == 0 || height == 0)
        return 0;

    FrameBuffer *fb = &core->frame_buffers[core->fb_count];
    fb->frame_buffer_id = core->fb_count + 1;
    fb->width = width;
    fb->height = height;
    fb->format = format;
    fb->pitch = width * ((format == PIXEL_FORMAT_RGB24) ? 3 : 4);
    fb->memory_size = (uint64_t)fb->pitch * height;
    fb->pixels = (uint8_t *)malloc(fb->memory_size);

    if (!fb->pixels)
        return 0;

    memset(fb->pixels, 0, fb->memory_size);
    fb->is_active = false;
    fb->refresh_rate = 60;
    fb->vsync_count = 0;

    core->fb_count++;
    return fb->frame_buffer_id;
}

int graphics_destroy_framebuffer(GraphicsCore *core, uint32_t fb_id)
{
    if (!core)
        return -1;

    for (uint32_t i = 0; i < core->fb_count; i++)
    {
        if (core->frame_buffers[i].frame_buffer_id == fb_id)
        {
            if (core->active_fb_id == fb_id)
            {
                core->active_fb_id = 0;
            }
            if (core->frame_buffers[i].pixels)
            {
                free(core->frame_buffers[i].pixels);
            }
            memmove(&core->frame_buffers[i], &core->frame_buffers[i + 1],
                    (core->fb_count - i - 1) * sizeof(FrameBuffer));
            core->fb_count--;
            return 0;
        }
    }
    return -1;
}

int graphics_set_active_framebuffer(GraphicsCore *core, uint32_t fb_id)
{
    if (!core)
        return -1;

    FrameBuffer *fb = graphics_get_framebuffer(core, fb_id);
    if (!fb)
        return -1;

    fb->is_active = true;
    core->active_fb_id = fb_id;
    return 0;
}

uint32_t graphics_create_surface(GraphicsCore *core, uint32_t width, uint32_t height,
                                 PixelFormat format, uint32_t parent_fb_id)
{
    if (!core || core->surface_count >= MAX_SURFACE_OBJECTS)
        return 0;
    if (width == 0 || height == 0)
        return 0;

    Surface *surface = &core->surfaces[core->surface_count];
    surface->surface_id = core->surface_count + 1;
    surface->width = width;
    surface->height = height;
    surface->format = format;
    surface->parent_fb_id = parent_fb_id;
    surface->blend_mode = BLEND_MODE_ALPHA;
    surface->use_color_key = false;

    uint32_t pixel_size = (format == PIXEL_FORMAT_RGB24) ? 3 : 4;
    uint32_t size = width * height * pixel_size;
    surface->pixels = (uint8_t *)malloc(size);

    if (!surface->pixels)
        return 0;

    memset(surface->pixels, 0, size);
    core->surface_count++;
    return surface->surface_id;
}

int graphics_destroy_surface(GraphicsCore *core, uint32_t surface_id)
{
    if (!core)
        return -1;

    for (uint32_t i = 0; i < core->surface_count; i++)
    {
        if (core->surfaces[i].surface_id == surface_id)
        {
            if (core->surfaces[i].pixels)
            {
                free(core->surfaces[i].pixels);
            }
            memmove(&core->surfaces[i], &core->surfaces[i + 1],
                    (core->surface_count - i - 1) * sizeof(Surface));
            core->surface_count--;
            return 0;
        }
    }
    return -1;
}

int graphics_clear_framebuffer(GraphicsCore *core, uint32_t fb_id, Color color)
{
    if (!core)
        return -1;

    FrameBuffer *fb = graphics_get_framebuffer(core, fb_id);
    if (!fb)
        return -1;

    for (uint32_t i = 0; i < fb->memory_size; i += 4)
    {
        fb->pixels[i] = color.r;
        fb->pixels[i + 1] = color.g;
        fb->pixels[i + 2] = color.b;
        fb->pixels[i + 3] = color.a;
    }

    core->total_operations++;
    return 0;
}

int graphics_fill_rect(GraphicsCore *core, uint32_t fb_id, Rect *rect, Color color)
{
    if (!core || !rect)
        return -1;

    FrameBuffer *fb = graphics_get_framebuffer(core, fb_id);
    if (!fb)
        return -1;

    for (int32_t y = rect->y; y < rect->y + rect->height; y++)
    {
        for (int32_t x = rect->x; x < rect->x + rect->width; x++)
        {
            if (x >= 0 && x < (int32_t)fb->width && y >= 0 && y < (int32_t)fb->height)
            {
                uint32_t offset = (y * fb->pitch) + (x * 4);
                fb->pixels[offset] = color.r;
                fb->pixels[offset + 1] = color.g;
                fb->pixels[offset + 2] = color.b;
                fb->pixels[offset + 3] = color.a;
                core->total_pixels_drawn++;
            }
        }
    }

    core->total_operations++;
    return 0;
}

int graphics_draw_line(GraphicsCore *core, uint32_t fb_id, Point *p1, Point *p2, Color color)
{
    if (!core || !p1 || !p2)
        return -1;

    FrameBuffer *fb = graphics_get_framebuffer(core, fb_id);
    if (!fb)
        return -1;

    int32_t dx = (p2->x > p1->x) ? (p2->x - p1->x) : (p1->x - p2->x);
    int32_t dy = (p2->y > p1->y) ? (p2->y - p1->y) : (p1->y - p2->y);
    int32_t steps = (dx > dy) ? dx : dy;

    if (steps == 0)
        steps = 1;

    double x_inc = (double)(p2->x - p1->x) / steps;
    double y_inc = (double)(p2->y - p1->y) / steps;
    double x = p1->x;
    double y = p1->y;

    for (int32_t i = 0; i <= steps; i++)
    {
        int32_t px = (int32_t)x;
        int32_t py = (int32_t)y;

        if (px >= 0 && px < (int32_t)fb->width && py >= 0 && py < (int32_t)fb->height)
        {
            uint32_t offset = (py * fb->pitch) + (px * 4);
            fb->pixels[offset] = color.r;
            fb->pixels[offset + 1] = color.g;
            fb->pixels[offset + 2] = color.b;
            fb->pixels[offset + 3] = color.a;
            core->total_pixels_drawn++;
        }

        x += x_inc;
        y += y_inc;
    }

    core->total_operations++;
    return 0;
}

int graphics_draw_circle(GraphicsCore *core, uint32_t fb_id, Point *center, int32_t radius, Color color)
{
    if (!core || !center || radius <= 0)
        return -1;

    FrameBuffer *fb = graphics_get_framebuffer(core, fb_id);
    if (!fb)
        return -1;

    for (int32_t y = -radius; y <= radius; y++)
    {
        for (int32_t x = -radius; x <= radius; x++)
        {
            int32_t dist_sq = x * x + y * y;
            int32_t radius_sq = radius * radius;

            if (dist_sq >= (radius_sq - radius) && dist_sq <= (radius_sq + radius))
            {
                int32_t px = center->x + x;
                int32_t py = center->y + y;

                if (px >= 0 && px < (int32_t)fb->width && py >= 0 && py < (int32_t)fb->height)
                {
                    uint32_t offset = (py * fb->pitch) + (px * 4);
                    fb->pixels[offset] = color.r;
                    fb->pixels[offset + 1] = color.g;
                    fb->pixels[offset + 2] = color.b;
                    fb->pixels[offset + 3] = color.a;
                    core->total_pixels_drawn++;
                }
            }
        }
    }

    core->total_operations++;
    return 0;
}

int graphics_fill_circle(GraphicsCore *core, uint32_t fb_id, Point *center, int32_t radius, Color color)
{
    if (!core || !center || radius <= 0)
        return -1;

    FrameBuffer *fb = graphics_get_framebuffer(core, fb_id);
    if (!fb)
        return -1;

    for (int32_t y = -radius; y <= radius; y++)
    {
        for (int32_t x = -radius; x <= radius; x++)
        {
            if (x * x + y * y <= radius * radius)
            {
                int32_t px = center->x + x;
                int32_t py = center->y + y;

                if (px >= 0 && px < (int32_t)fb->width && py >= 0 && py < (int32_t)fb->height)
                {
                    uint32_t offset = (py * fb->pitch) + (px * 4);
                    fb->pixels[offset] = color.r;
                    fb->pixels[offset + 1] = color.g;
                    fb->pixels[offset + 2] = color.b;
                    fb->pixels[offset + 3] = color.a;
                    core->total_pixels_drawn++;
                }
            }
        }
    }

    core->total_operations++;
    return 0;
}

int graphics_blit_surface(GraphicsCore *core, uint32_t src_surface_id, Rect *src_rect,
                          uint32_t dst_fb_id, Point *dst_point)
{
    if (!core || !src_rect || !dst_point)
        return -1;

    Surface *src = graphics_get_surface(core, src_surface_id);
    FrameBuffer *dst = graphics_get_framebuffer(core, dst_fb_id);
    if (!src || !dst)
        return -1;

    for (int32_t y = 0; y < src_rect->height; y++)
    {
        for (int32_t x = 0; x < src_rect->width; x++)
        {
            int32_t src_x = src_rect->x + x;
            int32_t src_y = src_rect->y + y;
            int32_t dst_x = dst_point->x + x;
            int32_t dst_y = dst_point->y + y;

            if (src_x >= 0 && src_x < (int32_t)src->width && src_y >= 0 && src_y < (int32_t)src->height &&
                dst_x >= 0 && dst_x < (int32_t)dst->width && dst_y >= 0 && dst_y < (int32_t)dst->height)
            {

                uint32_t src_offset = (src_y * src->width + src_x) * 4;
                uint32_t dst_offset = (dst_y * dst->pitch) + (dst_x * 4);

                memcpy(&dst->pixels[dst_offset], &src->pixels[src_offset], 4);
                core->total_pixels_drawn++;
            }
        }
    }

    core->total_operations++;
    return 0;
}

int graphics_blit_framebuffer(GraphicsCore *core, uint32_t src_fb_id, Rect *src_rect,
                              uint32_t dst_fb_id, Point *dst_point)
{
    if (!core || !src_rect || !dst_point)
        return -1;

    FrameBuffer *src = graphics_get_framebuffer(core, src_fb_id);
    FrameBuffer *dst = graphics_get_framebuffer(core, dst_fb_id);
    if (!src || !dst)
        return -1;

    for (int32_t y = 0; y < src_rect->height; y++)
    {
        for (int32_t x = 0; x < src_rect->width; x++)
        {
            int32_t src_x = src_rect->x + x;
            int32_t src_y = src_rect->y + y;
            int32_t dst_x = dst_point->x + x;
            int32_t dst_y = dst_point->y + y;

            if (src_x >= 0 && src_x < (int32_t)src->width && src_y >= 0 && src_y < (int32_t)src->height &&
                dst_x >= 0 && dst_x < (int32_t)dst->width && dst_y >= 0 && dst_y < (int32_t)dst->height)
            {

                uint32_t src_offset = (src_y * src->pitch) + (src_x * 4);
                uint32_t dst_offset = (dst_y * dst->pitch) + (dst_x * 4);

                memcpy(&dst->pixels[dst_offset], &src->pixels[src_offset], 4);
                core->total_pixels_drawn++;
            }
        }
    }

    core->total_operations++;
    return 0;
}

int graphics_set_blend_mode(GraphicsCore *core, uint32_t surface_id, BlendMode mode)
{
    if (!core)
        return -1;

    Surface *surface = graphics_get_surface(core, surface_id);
    if (!surface)
        return -1;

    surface->blend_mode = mode;
    return 0;
}

int graphics_set_color_key(GraphicsCore *core, uint32_t surface_id, Color key, bool enable)
{
    if (!core)
        return -1;

    Surface *surface = graphics_get_surface(core, surface_id);
    if (!surface)
        return -1;

    surface->color_key = key;
    surface->use_color_key = enable;
    return 0;
}

int graphics_present(GraphicsCore *core)
{
    if (!core)
        return -1;
    core->total_operations++;
    return 0;
}

int graphics_vsync_wait(GraphicsCore *core)
{
    if (!core)
        return -1;

    FrameBuffer *fb = graphics_get_framebuffer(core, core->active_fb_id);
    if (fb)
    {
        fb->vsync_count++;
    }
    return 0;
}

FrameBuffer *graphics_get_framebuffer(GraphicsCore *core, uint32_t fb_id)
{
    if (!core)
        return NULL;

    for (uint32_t i = 0; i < core->fb_count; i++)
    {
        if (core->frame_buffers[i].frame_buffer_id == fb_id)
        {
            return &core->frame_buffers[i];
        }
    }
    return NULL;
}

Surface *graphics_get_surface(GraphicsCore *core, uint32_t surface_id)
{
    if (!core)
        return NULL;

    for (uint32_t i = 0; i < core->surface_count; i++)
    {
        if (core->surfaces[i].surface_id == surface_id)
        {
            return &core->surfaces[i];
        }
    }
    return NULL;
}

uint32_t graphics_get_pixels_drawn(GraphicsCore *core)
{
    if (!core)
        return 0;
    return core->total_pixels_drawn;
}

uint32_t graphics_get_total_operations(GraphicsCore *core)
{
    if (!core)
        return 0;
    return core->total_operations;
}
