#include "etl_pipeline.h"
#include <stdlib.h>
#include <string.h>

ETLPipeline *etl_init(void)
{
    ETLPipeline *pipeline = (ETLPipeline *)malloc(sizeof(ETLPipeline));
    if (!pipeline)
        return NULL;

    memset(pipeline, 0, sizeof(ETLPipeline));
    pipeline->job_count = 0;
    pipeline->source_count = 0;
    pipeline->dest_count = 0;
    pipeline->rule_count = 0;

    return pipeline;
}

void etl_cleanup(ETLPipeline *pipeline)
{
    if (!pipeline)
        return;

    for (uint32_t i = 0; i < pipeline->job_count; i++)
    {
        // Cleanup transformations if needed
    }

    memset(pipeline, 0, sizeof(ETLPipeline));
    free(pipeline);
}

uint32_t etl_add_source(ETLPipeline *pipeline, const char *name, const char *type, const char *conn_str)
{
    if (!pipeline || !name || !type || !conn_str || pipeline->source_count >= MAX_SOURCES)
        return 0;

    DataSource *source = &pipeline->sources[pipeline->source_count];
    source->source_id = pipeline->source_count + 1;
    strncpy(source->source_name, name, 127);
    strncpy(source->source_type, type, 63);
    strncpy(source->connection_string, conn_str, 255);
    source->record_count = 0;
    source->last_read = time(NULL);

    pipeline->source_count++;
    return source->source_id;
}

uint32_t etl_add_destination(ETLPipeline *pipeline, const char *name, const char *type, const char *conn_str)
{
    if (!pipeline || !name || !type || !conn_str || pipeline->dest_count >= MAX_DESTINATIONS)
        return 0;

    DataDestination *dest = &pipeline->destinations[pipeline->dest_count];
    dest->dest_id = pipeline->dest_count + 1;
    strncpy(dest->dest_name, name, 127);
    strncpy(dest->dest_type, type, 63);
    strncpy(dest->connection_string, conn_str, 255);
    dest->records_written = 0;
    dest->last_written = time(NULL);

    pipeline->dest_count++;
    return dest->dest_id;
}

uint32_t etl_create_job(ETLPipeline *pipeline, const char *name, uint32_t source_id, uint32_t dest_id)
{
    if (!pipeline || !name || source_id == 0 || dest_id == 0 || pipeline->job_count >= MAX_ETL_JOBS)
        return 0;

    if (source_id > pipeline->source_count || dest_id > pipeline->dest_count)
        return 0;

    ETLJob *job = &pipeline->jobs[pipeline->job_count];
    job->job_id = pipeline->job_count + 1;
    strncpy(job->job_name, name, 127);
    job->source_id = source_id;
    job->dest_id = dest_id;
    job->status = JOB_STATUS_PENDING;
    job->transformation_count = 0;
    job->total_records_processed = 0;
    job->failed_records = 0;
    job->execution_time_ms = 0;
    job->success_rate = 0.0f;

    pipeline->job_count++;
    return job->job_id;
}

int etl_add_transformation(ETLPipeline *pipeline, uint32_t job_id, TransformationType type, const char *name)
{
    if (!pipeline || job_id == 0 || job_id > pipeline->job_count || !name)
        return -1;

    ETLJob *job = &pipeline->jobs[job_id - 1];

    if (job->transformation_count >= MAX_TRANSFORMATIONS)
        return -2;

    Transformation *transform = &job->transformations[job->transformation_count];
    transform->transform_id = job->transformation_count + 1;
    transform->job_id = job_id;
    transform->type = type;
    strncpy(transform->transform_name, name, 127);
    transform->input_records = 0;
    transform->output_records = 0;
    transform->error_records = 0;
    transform->error_rate = 0.0f;
    transform->enabled = true;

    job->transformation_count++;
    return 0;
}

uint32_t etl_add_validation_rule(ETLPipeline *pipeline, RuleType type, const char *name, const char *definition)
{
    if (!pipeline || !name || !definition || pipeline->rule_count >= MAX_RULES)
        return 0;

    ValidationRule *rule = &pipeline->rules[pipeline->rule_count];
    rule->rule_id = pipeline->rule_count + 1;
    rule->type = type;
    strncpy(rule->rule_name, name, 127);
    strncpy(rule->rule_definition, definition, 255);
    rule->enabled = true;
    rule->match_count = 0;
    rule->violation_count = 0;

    pipeline->rule_count++;
    return rule->rule_id;
}

int etl_apply_validation_rules(ETLPipeline *pipeline, uint32_t job_id)
{
    if (!pipeline || job_id == 0 || job_id > pipeline->job_count)
        return -1;

    ETLJob *job = &pipeline->jobs[job_id - 1];

    for (uint32_t i = 0; i < pipeline->rule_count; i++)
    {
        if (pipeline->rules[i].enabled)
        {
            pipeline->rules[i].match_count++;
        }
    }

    return 0;
}

int etl_start_job(ETLPipeline *pipeline, uint32_t job_id)
{
    if (!pipeline || job_id == 0 || job_id > pipeline->job_count)
        return -1;

    ETLJob *job = &pipeline->jobs[job_id - 1];
    job->status = JOB_STATUS_RUNNING;
    job->start_time = time(NULL);
    job->total_records_processed = 0;
    job->failed_records = 0;

    return 0;
}

int etl_stop_job(ETLPipeline *pipeline, uint32_t job_id)
{
    if (!pipeline || job_id == 0 || job_id > pipeline->job_count)
        return -1;

    ETLJob *job = &pipeline->jobs[job_id - 1];
    job->status = JOB_STATUS_COMPLETED;
    job->end_time = time(NULL);

    if (job->start_time > 0)
        job->execution_time_ms = (job->end_time - job->start_time) * 1000;

    if (job->total_records_processed > 0)
        job->success_rate = 100.0f * (float)(job->total_records_processed - job->failed_records) / (float)job->total_records_processed;

    return 0;
}

int etl_get_job_progress(ETLPipeline *pipeline, uint32_t job_id, float *progress, ETLJobStatus *status)
{
    if (!pipeline || job_id == 0 || job_id > pipeline->job_count || !progress || !status)
        return -1;

    ETLJob *job = &pipeline->jobs[job_id - 1];
    *status = job->status;
    *progress = (job->total_records_processed > 0) ? job->success_rate : 0.0f;

    return 0;
}

int etl_schedule_job(ETLPipeline *pipeline, uint32_t job_id, time_t schedule_time, uint32_t repeat_interval_minutes)
{
    if (!pipeline || job_id == 0 || job_id > pipeline->job_count)
        return -1;

    ETLJob *job = &pipeline->jobs[job_id - 1];

    if (schedule_time > time(NULL))
        job->status = JOB_STATUS_PENDING;

    return 0;
}

int etl_process_records(ETLPipeline *pipeline, uint32_t job_id, uint8_t *data, uint32_t size)
{
    if (!pipeline || job_id == 0 || job_id > pipeline->job_count || !data || size == 0)
        return -1;

    ETLJob *job = &pipeline->jobs[job_id - 1];

    if (job->status != JOB_STATUS_RUNNING)
        return -2;

    job->total_records_processed += (size / 100);

    for (uint32_t i = 0; i < job->transformation_count; i++)
    {
        job->transformations[i].input_records += (size / 100);
        job->transformations[i].output_records += (size / 100);
    }

    return 0;
}
