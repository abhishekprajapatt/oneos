#ifndef BUILD_LINKER_H
#define BUILD_LINKER_H

#include <stdint.h>

typedef struct
{
    uint32_t link_id;
    const char *object_file;
    const char *library_path;
    const char *output_binary;
    uint32_t link_status;
} build_linker_task_t;

typedef struct
{
    uint32_t total_links;
    uint32_t successful_links;
    uint32_t failed_links;
    uint64_t total_symbols_resolved;
} build_linker_state_t;

int build_linker_init(void);
int build_linker_link(build_linker_task_t *task);
int build_linker_get_symbols(uint32_t link_id, char *symbols, uint32_t *count);
int build_linker_cancel(uint32_t link_id);
int build_linker_get_status(uint32_t link_id, build_linker_task_t *task);
int build_linker_get_state(build_linker_state_t *state);

#endif
