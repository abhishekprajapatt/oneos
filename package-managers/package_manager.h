#ifndef PACKAGE_MANAGER_H
#define PACKAGE_MANAGER_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_PACKAGES 5000

typedef enum
{
    PKG_STATE_NOT_INSTALLED,
    PKG_STATE_INSTALLED,
    PKG_STATE_UPDATING,
    PKG_STATE_DAMAGED,
    PKG_STATE_DEPRECATED
} PackageState;

typedef struct
{
    char dependency_name[256];
    char min_version[32];
} Dependency;

typedef struct
{
    uint32_t package_id;
    char name[256];
    char version[32];
    char maintainer[256];
    char description[512];
    uint64_t size;
    PackageState state;
    uint32_t install_time;
    Dependency *dependencies;
    uint32_t dependency_count;
    char repository[256];
} Package;

typedef struct
{
    Package packages[MAX_PACKAGES];
    uint32_t package_count;
    char default_repo[256];
} PackageManager;

PackageManager *pkg_manager_create(const char *default_repo);
void pkg_manager_destroy(PackageManager *pm);
uint32_t pkg_manager_install_package(PackageManager *pm, const char *name, const char *version);
int pkg_manager_uninstall_package(PackageManager *pm, uint32_t pkg_id);
int pkg_manager_update_package(PackageManager *pm, uint32_t pkg_id, const char *new_version);
Package *pkg_manager_find_package(PackageManager *pm, const char *name);
int pkg_manager_search_packages(PackageManager *pm, const char *query, Package **results);
int pkg_manager_resolve_dependencies(PackageManager *pm, const char *package_name);
int pkg_manager_list_installed(PackageManager *pm, Package **packages);

#endif
