#ifndef ETL_PIPELINE_H
#define ETL_PIPELINE_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#define MAX_ETL_JOBS 256
#define MAX_TRANSFORMATIONS 512
#define MAX_SOURCES 128
#define MAX_DESTINATIONS 128
#define MAX_RULES 1024

typedef enum
{
    JOB_STATUS_PENDING,
    JOB_STATUS_RUNNING,
    JOB_STATUS_COMPLETED,
    JOB_STATUS_FAILED,
    JOB_STATUS_CANCELLED
} ETLJobStatus;

typedef enum
{
    TRANSFORM_CLEAN,
    TRANSFORM_VALIDATE,
    TRANSFORM_ENRICH,
    TRANSFORM_AGGREGATE,
    TRANSFORM_JOIN,
    TRANSFORM_FILTER,
    TRANSFORM_MAP,
    TRANSFORM_MERGE
} TransformationType;

typedef enum
{
    RULE_TYPE_FORMAT,
    RULE_TYPE_RANGE,
    RULE_TYPE_REGEX,
    RULE_TYPE_DUPLICATE,
    RULE_TYPE_MISSING,
    RULE_TYPE_REFERENCE,
    RULE_TYPE_BUSINESS
} RuleType;

typedef struct
{
    uint32_t source_id;
    char source_name[128];
    char source_type[64];
    char connection_string[256];
    uint64_t record_count;
    time_t last_read;
} DataSource;

typedef struct
{
    uint32_t dest_id;
    char dest_name[128];
    char dest_type[64];
    char connection_string[256];
    uint64_t records_written;
    time_t last_written;
} DataDestination;

typedef struct
{
    uint32_t rule_id;
    RuleType type;
    char rule_name[128];
    char rule_definition[256];
    bool enabled;
    uint32_t match_count;
    uint32_t violation_count;
} ValidationRule;

typedef struct
{
    uint32_t transform_id;
    uint32_t job_id;
    TransformationType type;
    char transform_name[128];
    uint64_t input_records;
    uint64_t output_records;
    uint64_t error_records;
    float error_rate;
    bool enabled;
} Transformation;

typedef struct
{
    uint32_t job_id;
    char job_name[128];
    uint32_t source_id;
    uint32_t dest_id;
    ETLJobStatus status;
    Transformation transformations[MAX_TRANSFORMATIONS];
    uint32_t transformation_count;
    uint64_t total_records_processed;
    uint64_t failed_records;
    time_t start_time;
    time_t end_time;
    uint64_t execution_time_ms;
    float success_rate;
} ETLJob;

typedef struct
{
    ETLJob jobs[MAX_ETL_JOBS];
    DataSource sources[MAX_SOURCES];
    DataDestination destinations[MAX_DESTINATIONS];
    ValidationRule rules[MAX_RULES];
    uint32_t job_count;
    uint32_t source_count;
    uint32_t dest_count;
    uint32_t rule_count;
} ETLPipeline;

ETLPipeline *etl_init(void);
void etl_cleanup(ETLPipeline *pipeline);

uint32_t etl_add_source(ETLPipeline *pipeline, const char *name, const char *type, const char *conn_str);
uint32_t etl_add_destination(ETLPipeline *pipeline, const char *name, const char *type, const char *conn_str);

uint32_t etl_create_job(ETLPipeline *pipeline, const char *name, uint32_t source_id, uint32_t dest_id);
int etl_add_transformation(ETLPipeline *pipeline, uint32_t job_id, TransformationType type, const char *name);

uint32_t etl_add_validation_rule(ETLPipeline *pipeline, RuleType type, const char *name, const char *definition);
int etl_apply_validation_rules(ETLPipeline *pipeline, uint32_t job_id);

int etl_start_job(ETLPipeline *pipeline, uint32_t job_id);
int etl_stop_job(ETLPipeline *pipeline, uint32_t job_id);
int etl_get_job_progress(ETLPipeline *pipeline, uint32_t job_id, float *progress, ETLJobStatus *status);

int etl_schedule_job(ETLPipeline *pipeline, uint32_t job_id, time_t schedule_time, uint32_t repeat_interval_minutes);
int etl_process_records(ETLPipeline *pipeline, uint32_t job_id, uint8_t *data, uint32_t size);

#endif
