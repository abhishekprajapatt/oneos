#ifndef AQUA_H
#define AQUA_H

#include <stdint.h>

int aqua_init(void);
int aqua_render_theme(const char *theme_name);
int aqua_set_appearance(uint32_t appearance);
int aqua_get_appearance(uint32_t *appearance);
int aqua_apply_effects(void *surface, uint32_t effect_type);

#endif
