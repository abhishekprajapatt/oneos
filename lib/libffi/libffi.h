#ifndef LIBFFI_H
#define LIBFFI_H

#include <stdint.h>

typedef struct
{
    uint32_t type_id;
    uint32_t size;
    uint32_t alignment;
} libffi_type_t;

int libffi_init(void);
int libffi_prep_cif(void *cif, uint32_t abi, void *args, void *rtype);
int libffi_call(void *cif, void *fn, void *rvalue, void **avalue);
int libffi_get_type(const char *type_name, libffi_type_t *type);
int libffi_sizeof_arg(uint32_t type);

#endif
