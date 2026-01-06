#include "libjson-c.h"

int libjson_c_init(void)
{
    return 0;
}

int libjson_c_parse(const char *json_str, json_object **obj)
{
    if (!json_str || !obj)
    {
        return -1;
    }
    return 0;
}

int libjson_c_to_string(json_object *obj, char *str, uint32_t max_len)
{
    if (!obj || !str || max_len == 0)
    {
        return -1;
    }
    return 0;
}

int libjson_c_object_new(json_object **obj)
{
    if (!obj)
    {
        return -1;
    }
    return 0;
}

int libjson_c_object_add(json_object *obj, const char *key, json_object *val)
{
    if (!obj || !key || !val)
    {
        return -1;
    }
    return 0;
}

int libjson_c_get_by_key(json_object *obj, const char *key, json_object **val)
{
    if (!obj || !key || !val)
    {
        return -1;
    }
    return 0;
}
