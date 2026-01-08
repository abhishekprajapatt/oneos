#ifndef BATCH_PROCESSOR_H
#define BATCH_PROCESSOR_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#define MAX_BATCH_PROCESSORS 32
#define MAX_BATCHES 256
#define MAX_BATCH_ITEMS 65536
#define MAX_PARTITIONS 128

typedef enum
{
    BATCH_STATUS_PENDING,
    BATCH_STATUS_PROCESSING,
    BATCH_STATUS_COMPLETED,
    BATCH_STATUS_FAILED,
    BATCH_STATUS_CANCELLED
} BatchStatus;

typedef enum
{
    PARTITION_STRATEGY_NONE,
    PARTITION_STRATEGY_RANGE,
    PARTITION_STRATEGY_HASH,
    PARTITION_STRATEGY_LIST,
    PARTITION_STRATEGY_ROUND_ROBIN
} PartitionStrategy;

typedef struct
{
    uint32_t item_id;
    uint8_t *data;
    uint32_t data_size;
    uint32_t sequence_number;
    bool processed;
    time_t timestamp;
} BatchItem;

typedef struct
{
    uint32_t batch_id;
    char batch_name[128];
    uint32_t processor_id;
    BatchStatus status;
    BatchItem *items;
    uint32_t item_count;
    uint32_t max_items;
    uint32_t completed_items;
    uint32_t failed_items;
    float progress_percent;
    time_t created_at;
    time_t started_at;
    time_t completed_at;
    uint64_t processing_time_ms;
} Batch;

typedef struct
{
    uint32_t partition_id;
    PartitionStrategy strategy;
    uint32_t partition_key;
    uint32_t item_count;
    uint32_t partition_size;
    float utilization_percent;
} Partition;

typedef struct
{
    uint32_t processor_id;
    char processor_name[128];
    uint32_t batch_count;
    uint32_t completed_batches;
    uint32_t failed_batches;
    uint64_t total_items_processed;
    uint32_t max_batch_size;
    uint32_t current_parallelism;
    uint32_t max_parallelism;
    float avg_processing_time_ms;
    bool active;
    time_t created_at;
} BatchProcessor;

typedef struct
{
    BatchProcessor processors[MAX_BATCH_PROCESSORS];
    Batch batches[MAX_BATCHES];
    Partition partitions[MAX_PARTITIONS];
    uint32_t processor_count;
    uint32_t batch_count;
    uint32_t partition_count;
} BatchProcessingFramework;

BatchProcessingFramework *batch_init(void);
void batch_cleanup(BatchProcessingFramework *framework);

uint32_t batch_create_processor(BatchProcessingFramework *framework, const char *name, uint32_t max_batch_size, uint32_t max_parallelism);
int batch_delete_processor(BatchProcessingFramework *framework, uint32_t processor_id);

uint32_t batch_create_batch(BatchProcessingFramework *framework, uint32_t processor_id, const char *name, uint32_t max_items);
int batch_add_item(BatchProcessingFramework *framework, uint32_t batch_id, const uint8_t *data, uint32_t size);
int batch_submit_batch(BatchProcessingFramework *framework, uint32_t batch_id);

int batch_process_items(BatchProcessingFramework *framework, uint32_t batch_id);
int batch_get_progress(BatchProcessingFramework *framework, uint32_t batch_id, float *progress, BatchStatus *status);
int batch_cancel_batch(BatchProcessingFramework *framework, uint32_t batch_id);

int batch_add_partition(BatchProcessingFramework *framework, uint32_t processor_id, PartitionStrategy strategy);
int batch_distribute_to_partitions(BatchProcessingFramework *framework, uint32_t batch_id);
int batch_get_partition_stats(BatchProcessingFramework *framework, uint32_t processor_id, uint32_t partition_id, float *utilization);

int batch_get_processor_stats(BatchProcessingFramework *framework, uint32_t processor_id, uint64_t *total_processed, float *avg_time);

#endif
