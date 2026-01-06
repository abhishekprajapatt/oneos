#ifndef SECURITY_H
#define SECURITY_H

#include <stdint.h>

typedef struct
{
    uint32_t uid;
    uint32_t gid;
    uint32_t permissions;
} security_context_t;

int security_init(void);
int security_check_permission(security_context_t *ctx, uint32_t required);
int security_validate_path(const char *path);
int security_encrypt(const void *data, uint32_t size, void *encrypted);
int security_decrypt(const void *encrypted, uint32_t size, void *data);
int security_get_context(uint32_t pid, security_context_t *ctx);

#endif
