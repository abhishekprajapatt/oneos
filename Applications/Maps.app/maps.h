#ifndef MAPS_H
#define MAPS_H

#include <stdint.h>

typedef struct
{
    double latitude;
    double longitude;
    const char *address;
    const char *place_name;
} location_t;

typedef struct
{
    location_t start;
    location_t end;
    double distance;
    uint32_t estimated_time;
    const char *route_type;
} route_t;

int maps_init(void);
int maps_search_location(const char *query, location_t *location);
int maps_get_route(location_t start, location_t end, route_t *route);
int maps_get_nearby_places(location_t center, double radius, location_t *places, uint32_t *count);
int maps_add_marker(location_t location, const char *marker_name);
int maps_remove_marker(uint32_t marker_id);
int maps_set_zoom_level(uint32_t zoom);
int maps_get_current_location(location_t *location);

#endif
