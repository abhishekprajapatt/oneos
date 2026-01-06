#ifndef DEPENDENCY_RESOLVER_H
#define DEPENDENCY_RESOLVER_H

#include <stdint.h>

typedef struct
{
    const char *package_name;
    const char *version;
    const char **dependencies;
    uint32_t dep_count;
} dependency_info_t;

int dependency_resolver_init(void);
int dependency_resolver_resolve(const char *package_name, dependency_info_t *info);
int dependency_resolver_check_circular(const char *package_name);
int dependency_resolver_install_chain(const char **packages, uint32_t count);
int dependency_resolver_get_order(const char **packages, uint32_t count, char **order, uint32_t *order_count);
int dependency_resolver_verify(const char *package_name);

#endif
