#ifndef PRESENTATION_H
#define PRESENTATION_H

#include <stdint.h>

typedef struct
{
    uint32_t slide_id;
    uint32_t slide_number;
    const char *slide_title;
    const char *slide_content;
    uint32_t total_elements;
} slide_info_t;

typedef struct
{
    uint32_t total_slides;
    const char *presentation_title;
    uint32_t current_slide;
} presentation_state_t;

int presentation_init(void);
int presentation_create(const char *title);
int presentation_open(const char *file_path);
int presentation_save(void);
int presentation_add_slide(slide_info_t *slide);
int presentation_delete_slide(uint32_t slide_number);
int presentation_goto_slide(uint32_t slide_number);
int presentation_start_slideshow(void);
int presentation_end_slideshow(void);
int presentation_export(const char *format);

#endif
