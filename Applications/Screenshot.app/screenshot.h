#ifndef SCREENSHOT_H
#define SCREENSHOT_H

#include <stdint.h>

typedef struct
{
    uint32_t screenshot_id;
    const char *file_path;
    uint32_t width;
    uint32_t height;
    uint32_t timestamp;
} screenshot_info_t;

int screenshot_init(void);
int screenshot_capture_full_screen(const char *output_path);
int screenshot_capture_region(uint32_t x, uint32_t y, uint32_t width, uint32_t height, const char *output_path);
int screenshot_capture_window(uint32_t window_id, const char *output_path);
int screenshot_open_editor(const char *file_path);
int screenshot_copy_to_clipboard(const char *file_path);
int screenshot_list_recent(screenshot_info_t *screenshots, uint32_t *count);
int screenshot_delete(uint32_t screenshot_id);

#endif
