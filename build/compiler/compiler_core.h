#ifndef COMPILER_CORE_H
#define COMPILER_CORE_H

#include <stdint.h>
#include <stdbool.h>

typedef enum
{
    LANG_C,
    LANG_CXX,
    LANG_RUST,
    LANG_ASM,
    LANG_PYTHON,
    LANG_GO
} SourceLanguage;

typedef enum
{
    OPT_NONE,
    OPT_O0,
    OPT_O1,
    OPT_O2,
    OPT_O3,
    OPT_Os,
    OPT_Oz
} OptimizationLevel;

typedef struct
{
    SourceLanguage language;
    OptimizationLevel opt_level;
    bool enable_debug_info;
    bool enable_lto;
    bool enable_pgo;
    char target_arch[64];
    char output_file[256];
} CompileOptions;

typedef struct
{
    uint32_t error_count;
    uint32_t warning_count;
    char error_message[1024];
    bool compilation_success;
    uint64_t compile_time_ms;
} CompileResult;

typedef struct
{
    char source_file[512];
    CompileOptions options;
    CompileResult result;
} CompilationJob;

typedef struct
{
    CompilationJob *jobs;
    uint32_t job_count;
    uint32_t max_jobs;
} CompilerCore;

CompilerCore *compiler_init(void);
void compiler_cleanup(CompilerCore *compiler);
uint32_t compiler_create_job(CompilerCore *compiler, const char *source_file, CompileOptions options);
CompileResult *compiler_compile_job(CompilerCore *compiler, uint32_t job_id);
int compiler_link_objects(CompilerCore *compiler, const char **object_files, uint32_t count, const char *output);
void compiler_set_optimization(CompilerCore *compiler, uint32_t job_id, OptimizationLevel level);
void compiler_enable_debug(CompilerCore *compiler, uint32_t job_id, bool enable);

#endif
