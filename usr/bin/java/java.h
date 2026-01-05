#ifndef JAVA_H
#define JAVA_H

#include <stdint.h>

typedef struct
{
    const char *class_name;
    const char *classpath;
    uint32_t heap_size;
    char **jvm_args;
    uint32_t arg_count;
} java_execution_options_t;

typedef struct
{
    const char *java_version;
    const char *vendor;
    const char *vm_name;
    uint64_t max_memory;
    uint64_t free_memory;
} java_runtime_info_t;

int java_init(void);
int java_execute_class(const char *class_name);
int java_execute_jar(const char *jar_file);
int java_execute_with_options(java_execution_options_t *options);
int java_get_runtime_info(java_runtime_info_t *info);
int java_compile_source(const char *source_file, const char *output_dir);

#endif
