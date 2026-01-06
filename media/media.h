#ifndef MEDIA_H
#define MEDIA_H

#include <stdint.h>

typedef struct
{
    const char *filename;
    uint32_t width;
    uint32_t height;
    uint32_t duration;
} media_info_t;

int media_init(void);
int media_load(const char *filename, media_info_t *info);
int media_play(void);
int media_pause(void);
int media_stop(void);
int media_seek(uint32_t position);

#endif
