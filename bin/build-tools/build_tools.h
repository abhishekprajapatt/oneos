#ifndef BUILD_TOOLS_H
#define BUILD_TOOLS_H

#include <stdint.h>

typedef struct
{
    uint32_t tool_id;
    const char *tool_name;
    const char *version;
} build_tools_config_t;

int build_tools_init(void);
int build_tools_compile(const char *source_file, const char *output_file);
int build_tools_link(const char *object_file, const char *executable);
int build_tools_optimize(const char *source_file);
int build_tools_validate_syntax(const char *source_file);
int build_tools_generate_debug_symbols(const char *executable);

#endif
