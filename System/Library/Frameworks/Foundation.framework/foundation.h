#ifndef FOUNDATION_H
#define FOUNDATION_H

#include <stdint.h>

typedef void NSString;
typedef void NSArray;
typedef void NSDictionary;

int foundation_init(void);
int foundation_create_string(const char *cstr, NSString **str);
int foundation_create_array(void **array);
int foundation_create_dictionary(void **dict);
int foundation_array_add_object(NSArray *array, void *object);
int foundation_dict_set_value(NSDictionary *dict, const char *key, void *value);

#endif
