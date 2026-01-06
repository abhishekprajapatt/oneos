#ifndef COREGRAPHICS_H
#define COREGRAPHICS_H

#include <stdint.h>

typedef void CGContext;
typedef void CGImage;

int coregraphics_init(void);
int coregraphics_create_context(uint32_t width, uint32_t height, CGContext **ctx);
int coregraphics_release_context(CGContext *ctx);
int coregraphics_draw_line(CGContext *ctx, float x1, float y1, float x2, float y2);
int coregraphics_fill_rect(CGContext *ctx, float x, float y, float width, float height);
int coregraphics_set_color(CGContext *ctx, uint32_t color);

#endif
