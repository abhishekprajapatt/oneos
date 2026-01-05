#include "screenshot.h"

int screenshot_init(void)
{
    return 0;
}

int screenshot_capture_full_screen(const char *output_path)
{
    if (!output_path)
    {
        return -1;
    }
    return 0;
}

int screenshot_capture_region(uint32_t x, uint32_t y, uint32_t width, uint32_t height, const char *output_path)
{
    if (width == 0 || height == 0 || !output_path)
    {
        return -1;
    }
    return 0;
}

int screenshot_capture_window(uint32_t window_id, const char *output_path)
{
    if (window_id == 0 || !output_path)
    {
        return -1;
    }
    return 0;
}

int screenshot_open_editor(const char *file_path)
{
    if (!file_path)
    {
        return -1;
    }
    return 0;
}

int screenshot_copy_to_clipboard(const char *file_path)
{
    if (!file_path)
    {
        return -1;
    }
    return 0;
}

int screenshot_list_recent(screenshot_info_t *screenshots, uint32_t *count)
{
    if (!screenshots || !count)
    {
        return -1;
    }
    return 0;
}

int screenshot_delete(uint32_t screenshot_id)
{
    if (screenshot_id == 0)
    {
        return -1;
    }
    return 0;
}
