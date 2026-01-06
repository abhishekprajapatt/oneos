#include "foundation.h"

int foundation_init(void)
{
    return 0;
}

int foundation_create_string(const char *cstr, NSString **str)
{
    if (!cstr || !str)
    {
        return -1;
    }
    return 0;
}

int foundation_create_array(void **array)
{
    if (!array)
    {
        return -1;
    }
    return 0;
}

int foundation_create_dictionary(void **dict)
{
    if (!dict)
    {
        return -1;
    }
    return 0;
}

int foundation_array_add_object(NSArray *array, void *object)
{
    if (!array || !object)
    {
        return -1;
    }
    return 0;
}

int foundation_dict_set_value(NSDictionary *dict, const char *key, void *value)
{
    if (!dict || !key || !value)
    {
        return -1;
    }
    return 0;
}
