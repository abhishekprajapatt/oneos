#include "metal.h"

int metal_init(void)
{
    return 0;
}

int metal_create_device(MTLDevice **device)
{
    if (!device)
    {
        return -1;
    }
    return 0;
}

int metal_create_command_queue(MTLDevice *device, MTLCommandQueue **queue)
{
    if (!device || !queue)
    {
        return -1;
    }
    return 0;
}

int metal_compile_shader(const char *shader_source, void **shader)
{
    if (!shader_source || !shader)
    {
        return -1;
    }
    return 0;
}

int metal_draw_call(MTLCommandQueue *queue, void *shader, uint32_t vertex_count)
{
    if (!queue || !shader || vertex_count == 0)
    {
        return -1;
    }
    return 0;
}

int metal_present(void)
{
    return 0;
}
