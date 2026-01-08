#ifndef GRAPHICS_CORE_H
#define GRAPHICS_CORE_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_FRAME_BUFFERS 16
#define MAX_SURFACE_OBJECTS 512
#define MAX_DRAWING_OPERATIONS 4096

typedef enum
{
    PIXEL_FORMAT_RGB24 = 0,
    PIXEL_FORMAT_RGBA32 = 1,
    PIXEL_FORMAT_BGRA32 = 2,
    PIXEL_FORMAT_RGB565 = 3,
    PIXEL_FORMAT_GRAYSCALE = 4
} PixelFormat;

typedef enum
{
    BLEND_MODE_REPLACE = 0,
    BLEND_MODE_ALPHA = 1,
    BLEND_MODE_ADD = 2,
    BLEND_MODE_MULTIPLY = 3,
    BLEND_MODE_SCREEN = 4
} BlendMode;

typedef struct
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} Color;

typedef struct
{
    int32_t x;
    int32_t y;
    int32_t width;
    int32_t height;
} Rect;

typedef struct
{
    int32_t x;
    int32_t y;
} Point;

typedef struct
{
    uint32_t frame_buffer_id;
    uint32_t width;
    uint32_t height;
    PixelFormat format;
    uint32_t pitch;
    uint8_t *pixels;
    uint64_t memory_size;
    bool is_active;
    uint32_t refresh_rate;
    uint32_t vsync_count;
} FrameBuffer;

typedef struct
{
    uint32_t surface_id;
    uint32_t width;
    uint32_t height;
    PixelFormat format;
    uint8_t *pixels;
    BlendMode blend_mode;
    Color color_key;
    bool use_color_key;
    uint32_t parent_fb_id;
} Surface;

typedef struct
{
    FrameBuffer frame_buffers[MAX_FRAME_BUFFERS];
    uint32_t fb_count;
    uint32_t active_fb_id;
    Surface surfaces[MAX_SURFACE_OBJECTS];
    uint32_t surface_count;
    uint32_t total_pixels_drawn;
    uint32_t total_operations;
    Color clear_color;
} GraphicsCore;

GraphicsCore *graphics_init(void);
void graphics_cleanup(GraphicsCore *core);

uint32_t graphics_create_framebuffer(GraphicsCore *core, uint32_t width,
                                     uint32_t height, PixelFormat format);
int graphics_destroy_framebuffer(GraphicsCore *core, uint32_t fb_id);
int graphics_set_active_framebuffer(GraphicsCore *core, uint32_t fb_id);

uint32_t graphics_create_surface(GraphicsCore *core, uint32_t width, uint32_t height,
                                 PixelFormat format, uint32_t parent_fb_id);
int graphics_destroy_surface(GraphicsCore *core, uint32_t surface_id);

int graphics_clear_framebuffer(GraphicsCore *core, uint32_t fb_id, Color color);
int graphics_fill_rect(GraphicsCore *core, uint32_t fb_id, Rect *rect, Color color);
int graphics_draw_line(GraphicsCore *core, uint32_t fb_id, Point *p1, Point *p2, Color color);
int graphics_draw_circle(GraphicsCore *core, uint32_t fb_id, Point *center, int32_t radius, Color color);
int graphics_fill_circle(GraphicsCore *core, uint32_t fb_id, Point *center, int32_t radius, Color color);

int graphics_blit_surface(GraphicsCore *core, uint32_t src_surface_id, Rect *src_rect,
                          uint32_t dst_fb_id, Point *dst_point);
int graphics_blit_framebuffer(GraphicsCore *core, uint32_t src_fb_id, Rect *src_rect,
                              uint32_t dst_fb_id, Point *dst_point);

int graphics_set_blend_mode(GraphicsCore *core, uint32_t surface_id, BlendMode mode);
int graphics_set_color_key(GraphicsCore *core, uint32_t surface_id, Color key, bool enable);

int graphics_present(GraphicsCore *core);
int graphics_vsync_wait(GraphicsCore *core);

FrameBuffer *graphics_get_framebuffer(GraphicsCore *core, uint32_t fb_id);
Surface *graphics_get_surface(GraphicsCore *core, uint32_t surface_id);

uint32_t graphics_get_pixels_drawn(GraphicsCore *core);
uint32_t graphics_get_total_operations(GraphicsCore *core);

#endif
