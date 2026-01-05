#ifndef WEATHER_H
#define WEATHER_H

#include <stdint.h>

typedef struct
{
    double latitude;
    double longitude;
    const char *city_name;
} location_weather_t;

typedef struct
{
    uint32_t temperature;
    uint32_t humidity;
    uint32_t wind_speed;
    const char *condition;
    uint32_t uv_index;
} weather_data_t;

int weather_init(void);
int weather_get_current(location_weather_t location, weather_data_t *data);
int weather_get_forecast(location_weather_t location, weather_data_t *forecast, uint32_t days);
int weather_add_location(const char *city_name);
int weather_remove_location(const char *city_name);
int weather_list_locations(char **locations, uint32_t *count);
int weather_search_location(const char *query, location_weather_t *locations, uint32_t *count);
int weather_get_alerts(location_weather_t location, char *alerts, uint32_t max_len);

#endif
