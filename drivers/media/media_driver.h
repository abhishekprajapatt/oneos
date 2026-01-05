#ifndef MEDIA_DRIVER_H
#define MEDIA_DRIVER_H

#include <stdint.h>

typedef struct
{
    uint32_t id;
    uint32_t type;
    uint32_t format;
} media_entity_t;

typedef struct
{
    uint32_t source_id;
    uint32_t sink_id;
    uint32_t flags;
} media_link_t;

int media_init(void);
int media_register_entity(media_entity_t *entity);
int media_create_link(media_link_t *link);
int media_setup_link(media_link_t *link, uint32_t flags);

#endif
