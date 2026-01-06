#include "libgccrt.h"

int libgccrt_init(void)
{
    return 0;
}

int libgccrt_register_exception_handler(void *handler)
{
    if (!handler)
    {
        return -1;
    }
    return 0;
}

int libgccrt_unwind_frame(void *context)
{
    if (!context)
    {
        return -1;
    }
    return 0;
}

int libgccrt_get_personality_routine(void **routine)
{
    if (!routine)
    {
        return -1;
    }
    return 0;
}

int libgccrt_dwarf_cfi_info(void *fde)
{
    if (!fde)
    {
        return -1;
    }
    return 0;
}
