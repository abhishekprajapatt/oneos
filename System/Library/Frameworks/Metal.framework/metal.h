#ifndef METAL_H
#define METAL_H

#include <stdint.h>

typedef void MTLDevice;
typedef void MTLCommandQueue;

int metal_init(void);
int metal_create_device(MTLDevice **device);
int metal_create_command_queue(MTLDevice *device, MTLCommandQueue **queue);
int metal_compile_shader(const char *shader_source, void **shader);
int metal_draw_call(MTLCommandQueue *queue, void *shader, uint32_t vertex_count);
int metal_present(void);

#endif
