#ifndef SYS_FONTS_H
#define SYS_FONTS_H

#include <stdint.h>

typedef struct
{
    const char *family;
    const char *style;
    uint32_t size;
} sys_font_t;

int sys_fonts_init(void);
int sys_fonts_register(sys_font_t *font);
int sys_fonts_unregister(const char *font_name);
int sys_fonts_list(sys_font_t **fonts, uint32_t *count);
int sys_fonts_get_default(sys_font_t *font);
int sys_fonts_set_default(const char *font_name);

#endif
