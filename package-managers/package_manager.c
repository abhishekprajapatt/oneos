#include "package_manager.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

PackageManager *pkg_manager_create(const char *default_repo)
{
    PackageManager *pm = (PackageManager *)malloc(sizeof(PackageManager));
    if (!pm)
        return NULL;

    pm->package_count = 0;
    memset(pm->packages, 0, sizeof(pm->packages));

    if (default_repo)
    {
        strncpy(pm->default_repo, default_repo, 255);
    }
    else
    {
        strcpy(pm->default_repo, "https://repo.oneos.local");
    }

    return pm;
}

void pkg_manager_destroy(PackageManager *pm)
{
    if (!pm)
        return;

    for (uint32_t i = 0; i < pm->package_count; i++)
    {
        if (pm->packages[i].dependencies)
        {
            free(pm->packages[i].dependencies);
        }
    }

    free(pm);
}

uint32_t pkg_manager_install_package(PackageManager *pm, const char *name, const char *version)
{
    if (!pm || pm->package_count >= MAX_PACKAGES || !name || !version)
        return 0;

    Package *pkg = &pm->packages[pm->package_count];
    pkg->package_id = pm->package_count + 1;
    strncpy(pkg->name, name, 255);
    strncpy(pkg->version, version, 31);
    pkg->state = PKG_STATE_INSTALLED;
    pkg->install_time = time(NULL);
    pkg->dependency_count = 0;
    pkg->dependencies = NULL;
    strcpy(pkg->repository, pm->default_repo);

    pm->package_count++;
    return pkg->package_id;
}

int pkg_manager_uninstall_package(PackageManager *pm, uint32_t pkg_id)
{
    if (!pm)
        return -1;

    for (uint32_t i = 0; i < pm->package_count; i++)
    {
        if (pm->packages[i].package_id == pkg_id)
        {
            if (pm->packages[i].dependencies)
            {
                free(pm->packages[i].dependencies);
            }
            for (uint32_t j = i; j < pm->package_count - 1; j++)
            {
                pm->packages[j] = pm->packages[j + 1];
            }
            pm->package_count--;
            return 0;
        }
    }

    return -1;
}

int pkg_manager_update_package(PackageManager *pm, uint32_t pkg_id, const char *new_version)
{
    if (!pm || !new_version)
        return -1;

    for (uint32_t i = 0; i < pm->package_count; i++)
    {
        if (pm->packages[i].package_id == pkg_id)
        {
            pm->packages[i].state = PKG_STATE_UPDATING;
            strncpy(pm->packages[i].version, new_version, 31);
            pm->packages[i].state = PKG_STATE_INSTALLED;
            return 0;
        }
    }

    return -1;
}

Package *pkg_manager_find_package(PackageManager *pm, const char *name)
{
    if (!pm || !name)
        return NULL;

    for (uint32_t i = 0; i < pm->package_count; i++)
    {
        if (strcmp(pm->packages[i].name, name) == 0)
        {
            return &pm->packages[i];
        }
    }

    return NULL;
}

int pkg_manager_search_packages(PackageManager *pm, const char *query, Package **results)
{
    if (!pm || !query || !results)
        return 0;

    uint32_t count = 0;
    for (uint32_t i = 0; i < pm->package_count; i++)
    {
        if (strstr(pm->packages[i].name, query) != NULL)
        {
            count++;
        }
    }

    return count;
}

int pkg_manager_resolve_dependencies(PackageManager *pm, const char *package_name)
{
    if (!pm || !package_name)
        return 0;

    Package *pkg = pkg_manager_find_package(pm, package_name);
    if (!pkg)
        return 0;

    return pkg->dependency_count;
}

int pkg_manager_list_installed(PackageManager *pm, Package **packages)
{
    if (!pm || !packages)
        return 0;

    uint32_t count = 0;
    for (uint32_t i = 0; i < pm->package_count; i++)
    {
        if (pm->packages[i].state == PKG_STATE_INSTALLED)
        {
            count++;
        }
    }

    return count;
}
