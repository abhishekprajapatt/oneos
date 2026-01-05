#ifndef ABI_LAYER_H
#define ABI_LAYER_H

#include <stdint.h>

typedef struct
{
    uint32_t abi_version;
    const char *abi_name;
    uint32_t abi_class;
} abi_info_t;

int abi_init(void);
int abi_register_interface(abi_info_t *abi);
int abi_check_compatibility(abi_info_t *abi1, abi_info_t *abi2);
int abi_translate_structure(const char *struct_name, void *src, void *dst);
int abi_get_calling_convention(char *convention, uint32_t max_len);
int abi_validate_binary(const char *binary_path);

#endif
