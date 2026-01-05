#include "pdf_reader.h"

int pdf_reader_init(void)
{
    return 0;
}

int pdf_reader_open_file(const char *file_path)
{
    if (!file_path)
    {
        return -1;
    }
    return 0;
}

int pdf_reader_close_file(void)
{
    return 0;
}

int pdf_reader_next_page(void)
{
    return 0;
}

int pdf_reader_previous_page(void)
{
    return 0;
}

int pdf_reader_goto_page(uint32_t page_number)
{
    if (page_number == 0)
    {
        return -1;
    }
    return 0;
}

int pdf_reader_zoom_in(void)
{
    return 0;
}

int pdf_reader_zoom_out(void)
{
    return 0;
}

int pdf_reader_set_zoom(double zoom_level)
{
    if (zoom_level <= 0)
    {
        return -1;
    }
    return 0;
}

int pdf_reader_search_text(const char *search_term)
{
    if (!search_term)
    {
        return -1;
    }
    return 0;
}
