#ifndef PDF_READER_H
#define PDF_READER_H

#include <stdint.h>

typedef struct
{
    uint32_t page_number;
    uint32_t total_pages;
    double zoom_level;
    const char *document_title;
} pdf_viewer_state_t;

int pdf_reader_init(void);
int pdf_reader_open_file(const char *file_path);
int pdf_reader_close_file(void);
int pdf_reader_next_page(void);
int pdf_reader_previous_page(void);
int pdf_reader_goto_page(uint32_t page_number);
int pdf_reader_zoom_in(void);
int pdf_reader_zoom_out(void);
int pdf_reader_set_zoom(double zoom_level);
int pdf_reader_search_text(const char *search_term);

#endif
