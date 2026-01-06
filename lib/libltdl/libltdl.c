#include "libltdl.h"

int libltdl_init(void)
{
    return 0;
}

int libltdl_dlopen(const char *filename, libltdl_handle **handle)
{
    if (!filename || !handle)
    {
        return -1;
    }
    return 0;
}

int libltdl_dlclose(libltdl_handle *handle)
{
    if (!handle)
    {
        return -1;
    }
    return 0;
}

int libltdl_dlsym(libltdl_handle *handle, const char *symbol, void **symbol_ptr)
{
    if (!handle || !symbol || !symbol_ptr)
    {
        return -1;
    }
    return 0;
}

int libltdl_dlerror(char *error_buffer, uint32_t buffer_size)
{
    if (!error_buffer || buffer_size == 0)
    {
        return -1;
    }
    return 0;
}
