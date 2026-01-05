#ifndef DISPLAY_LCD_H
#define DISPLAY_LCD_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    uint32_t base_address;
    uint32_t width;
    uint32_t height;
    uint8_t bpp;
} display_lcd_device_t;

typedef struct
{
    uint32_t htiming;
    uint32_t vtiming;
    uint32_t polarity;
    uint32_t clk_div;
} display_lcd_timing_t;

int display_lcd_init(void);
int display_lcd_probe(display_lcd_device_t *dev);
int display_lcd_set_timing(display_lcd_device_t *dev, display_lcd_timing_t *timing);
int display_lcd_enable(display_lcd_device_t *dev);
int display_lcd_disable(display_lcd_device_t *dev);
int display_lcd_set_backlight(display_lcd_device_t *dev, uint8_t level);

#endif
