#include "batch_processor.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

BatchProcessingFramework *batch_init(void)
{
    BatchProcessingFramework *framework = (BatchProcessingFramework *)malloc(sizeof(BatchProcessingFramework));
    if (!framework)
        return NULL;

    memset(framework, 0, sizeof(BatchProcessingFramework));
    framework->processor_count = 0;
    framework->batch_count = 0;
    framework->partition_count = 0;

    return framework;
}

void batch_cleanup(BatchProcessingFramework *framework)
{
    if (!framework)
        return;

    for (uint32_t i = 0; i < framework->batch_count; i++)
    {
        if (framework->batches[i].items)
            free(framework->batches[i].items);
    }

    memset(framework, 0, sizeof(BatchProcessingFramework));
    free(framework);
}

uint32_t batch_create_processor(BatchProcessingFramework *framework, const char *name, uint32_t max_batch_size, uint32_t max_parallelism)
{
    if (!framework || !name || framework->processor_count >= MAX_BATCH_PROCESSORS)
        return 0;

    BatchProcessor *processor = &framework->processors[framework->processor_count];
    processor->processor_id = framework->processor_count + 1;
    strncpy(processor->processor_name, name, 127);
    processor->batch_count = 0;
    processor->completed_batches = 0;
    processor->failed_batches = 0;
    processor->total_items_processed = 0;
    processor->max_batch_size = max_batch_size;
    processor->current_parallelism = 0;
    processor->max_parallelism = max_parallelism;
    processor->avg_processing_time_ms = 0.0f;
    processor->active = true;
    processor->created_at = time(NULL);

    framework->processor_count++;
    return processor->processor_id;
}

int batch_delete_processor(BatchProcessingFramework *framework, uint32_t processor_id)
{
    if (!framework || processor_id == 0 || processor_id > framework->processor_count)
        return -1;

    BatchProcessor *processor = &framework->processors[processor_id - 1];
    processor->active = false;

    return 0;
}

uint32_t batch_create_batch(BatchProcessingFramework *framework, uint32_t processor_id, const char *name, uint32_t max_items)
{
    if (!framework || processor_id == 0 || processor_id > framework->processor_count || !name || framework->batch_count >= MAX_BATCHES)
        return 0;

    Batch *batch = &framework->batches[framework->batch_count];
    batch->batch_id = framework->batch_count + 1;
    batch->processor_id = processor_id;
    strncpy(batch->batch_name, name, 127);
    batch->status = BATCH_STATUS_PENDING;
    batch->max_items = max_items;
    batch->item_count = 0;
    batch->completed_items = 0;
    batch->failed_items = 0;
    batch->progress_percent = 0.0f;
    batch->items = (BatchItem *)malloc(sizeof(BatchItem) * max_items);
    batch->created_at = time(NULL);
    batch->processing_time_ms = 0;

    framework->batch_count++;
    framework->processors[processor_id - 1].batch_count++;

    return batch->batch_id;
}

int batch_add_item(BatchProcessingFramework *framework, uint32_t batch_id, const uint8_t *data, uint32_t size)
{
    if (!framework || batch_id == 0 || batch_id > framework->batch_count || !data || size == 0)
        return -1;

    Batch *batch = &framework->batches[batch_id - 1];

    if (batch->item_count >= batch->max_items)
        return -2;

    BatchItem *item = &batch->items[batch->item_count];
    item->item_id = batch->item_count + 1;
    item->data = (uint8_t *)malloc(size);
    memcpy(item->data, data, size);
    item->data_size = size;
    item->sequence_number = batch->item_count;
    item->processed = false;
    item->timestamp = time(NULL);

    batch->item_count++;

    return 0;
}

int batch_submit_batch(BatchProcessingFramework *framework, uint32_t batch_id)
{
    if (!framework || batch_id == 0 || batch_id > framework->batch_count)
        return -1;

    Batch *batch = &framework->batches[batch_id - 1];
    batch->status = BATCH_STATUS_PROCESSING;
    batch->started_at = time(NULL);

    return 0;
}

int batch_process_items(BatchProcessingFramework *framework, uint32_t batch_id)
{
    if (!framework || batch_id == 0 || batch_id > framework->batch_count)
        return -1;

    Batch *batch = &framework->batches[batch_id - 1];

    for (uint32_t i = 0; i < batch->item_count; i++)
    {
        if (!batch->items[i].processed)
        {
            batch->items[i].processed = true;
            batch->completed_items++;
        }
    }

    batch->progress_percent = 100.0f * (float)batch->completed_items / (float)batch->item_count;
    batch->status = BATCH_STATUS_COMPLETED;
    batch->completed_at = time(NULL);

    if (batch->started_at > 0)
        batch->processing_time_ms = (batch->completed_at - batch->started_at) * 1000;

    return 0;
}

int batch_get_progress(BatchProcessingFramework *framework, uint32_t batch_id, float *progress, BatchStatus *status)
{
    if (!framework || batch_id == 0 || batch_id > framework->batch_count || !progress || !status)
        return -1;

    Batch *batch = &framework->batches[batch_id - 1];
    *progress = batch->progress_percent;
    *status = batch->status;

    return 0;
}

int batch_cancel_batch(BatchProcessingFramework *framework, uint32_t batch_id)
{
    if (!framework || batch_id == 0 || batch_id > framework->batch_count)
        return -1;

    Batch *batch = &framework->batches[batch_id - 1];
    batch->status = BATCH_STATUS_CANCELLED;

    for (uint32_t i = 0; i < batch->item_count; i++)
    {
        if (batch->items[i].data)
            free(batch->items[i].data);
    }

    return 0;
}

int batch_add_partition(BatchProcessingFramework *framework, uint32_t processor_id, PartitionStrategy strategy)
{
    if (!framework || processor_id == 0 || processor_id > framework->processor_count || framework->partition_count >= MAX_PARTITIONS)
        return -1;

    Partition *partition = &framework->partitions[framework->partition_count];
    partition->partition_id = framework->partition_count + 1;
    partition->strategy = strategy;
    partition->partition_key = 0;
    partition->item_count = 0;
    partition->partition_size = 0;
    partition->utilization_percent = 0.0f;

    framework->partition_count++;

    return 0;
}

int batch_distribute_to_partitions(BatchProcessingFramework *framework, uint32_t batch_id)
{
    if (!framework || batch_id == 0 || batch_id > framework->batch_count)
        return -1;

    Batch *batch = &framework->batches[batch_id - 1];
    uint32_t items_per_partition = (batch->item_count + framework->partition_count - 1) / framework->partition_count;

    for (uint32_t i = 0; i < framework->partition_count; i++)
    {
        framework->partitions[i].item_count = (i < framework->partition_count - 1) ? items_per_partition : batch->item_count - (i * items_per_partition);
        framework->partitions[i].utilization_percent = 100.0f * (float)framework->partitions[i].item_count / (float)items_per_partition;
    }

    return 0;
}

int batch_get_partition_stats(BatchProcessingFramework *framework, uint32_t processor_id, uint32_t partition_id, float *utilization)
{
    if (!framework || processor_id == 0 || processor_id > framework->processor_count || partition_id == 0 || partition_id > framework->partition_count || !utilization)
        return -1;

    Partition *partition = &framework->partitions[partition_id - 1];
    *utilization = partition->utilization_percent;

    return 0;
}

int batch_get_processor_stats(BatchProcessingFramework *framework, uint32_t processor_id, uint64_t *total_processed, float *avg_time)
{
    if (!framework || processor_id == 0 || processor_id > framework->processor_count || !total_processed || !avg_time)
        return -1;

    BatchProcessor *processor = &framework->processors[processor_id - 1];
    *total_processed = processor->total_items_processed;
    *avg_time = processor->avg_processing_time_ms;

    return 0;
}
