#include "build_system.h"

int build_system_init(void)
{
    return 0;
}

int build_system_compile(const char *source_file, const char *output_file)
{
    if (!source_file || !output_file)
    {
        return -1;
    }
    return 0;
}

int build_system_link(const char **object_files, uint32_t count, const char *output)
{
    if (!object_files || count == 0 || !output)
    {
        return -1;
    }
    return 0;
}

int build_system_clean(const char *directory)
{
    if (!directory)
    {
        return -1;
    }
    return 0;
}

int build_system_parallel_build(uint32_t num_jobs)
{
    if (num_jobs == 0)
    {
        return -1;
    }
    return 0;
}

int build_system_get_status(uint32_t *progress)
{
    if (!progress)
    {
        return -1;
    }
    *progress = 0;
    return 0;
}
