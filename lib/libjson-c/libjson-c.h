#ifndef LIBJSON_C_H
#define LIBJSON_C_H

#include <stdint.h>

typedef void json_object;

int libjson_c_init(void);
int libjson_c_parse(const char *json_str, json_object **obj);
int libjson_c_to_string(json_object *obj, char *str, uint32_t max_len);
int libjson_c_object_new(json_object **obj);
int libjson_c_object_add(json_object *obj, const char *key, json_object *val);
int libjson_c_get_by_key(json_object *obj, const char *key, json_object **val);

#endif
