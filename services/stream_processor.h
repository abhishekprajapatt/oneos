#ifndef STREAM_PROCESSOR_H
#define STREAM_PROCESSOR_H

#include <stdint.h>
#include <stddef.h>
#include <time.h>

#define MAX_STREAMS 128
#define MAX_STREAM_BUFFER 16384
#define MAX_STREAM_PARTITIONS 32
#define MAX_CONSUMERS 256
#define MAX_TOPICS 64

typedef enum
{
    STREAM_SOURCE_NETWORK = 0,
    STREAM_SOURCE_FILE = 1,
    STREAM_SOURCE_SENSOR = 2,
    STREAM_SOURCE_DATABASE = 3,
    STREAM_SOURCE_APPLICATION = 4
} StreamSourceType;

typedef enum
{
    PARTITION_STRATEGY_ROUND_ROBIN = 0,
    PARTITION_STRATEGY_KEY_HASH = 1,
    PARTITION_STRATEGY_RANDOM = 2
} PartitionStrategy;

typedef struct
{
    uint64_t event_id;
    uint64_t timestamp;
    uint32_t key_hash;
    uint8_t *payload;
    uint32_t payload_size;
    uint8_t is_processed;
    uint32_t partition_id;
} StreamEvent;

typedef struct
{
    uint32_t partition_id;
    uint32_t event_count;
    StreamEvent *events;
    uint64_t offset;
    uint32_t capacity;
} StreamPartition;

typedef struct
{
    uint32_t topic_id;
    char topic_name[64];
    uint32_t partition_count;
    StreamPartition partitions[MAX_STREAM_PARTITIONS];
    PartitionStrategy partition_strategy;
    uint64_t total_events;
    uint64_t total_bytes;
    uint64_t last_event_time;
    uint32_t consumer_count;
    uint8_t retention_policy;
    uint32_t retention_hours;
} StreamTopic;

typedef struct
{
    uint32_t consumer_id;
    char consumer_group[64];
    uint32_t topic_id;
    uint32_t current_partition;
    uint64_t current_offset;
    uint64_t last_commit_offset;
    uint32_t batch_size;
    uint64_t processed_events;
    uint64_t processed_bytes;
    uint8_t is_active;
} StreamConsumer;

typedef struct
{
    uint32_t processor_id;
    char name[64];
    uint32_t topic_count;
    StreamTopic topics[MAX_TOPICS];
    uint32_t consumer_count;
    StreamConsumer consumers[MAX_CONSUMERS];
    uint64_t total_events_processed;
    uint64_t total_latency_ms;
    uint32_t peak_throughput_events_sec;
} StreamProcessor;

int sp_create_processor(const char *name);
int sp_delete_processor(uint32_t processor_id);

int sp_create_topic(uint32_t processor_id, const char *topic_name, uint32_t partition_count);
int sp_delete_topic(uint32_t processor_id, uint32_t topic_id);
int sp_set_topic_retention(uint32_t processor_id, uint32_t topic_id, uint32_t hours);

int sp_publish_event(uint32_t processor_id, uint32_t topic_id, const void *payload, uint32_t payload_size);
int sp_publish_with_key(uint32_t processor_id, uint32_t topic_id, const char *key, const void *payload, uint32_t payload_size);
int sp_publish_batch(uint32_t processor_id, uint32_t topic_id, const void **payloads, const uint32_t *sizes, uint32_t count);

int sp_create_consumer(uint32_t processor_id, uint32_t topic_id, const char *consumer_group, uint32_t batch_size);
int sp_delete_consumer(uint32_t processor_id, uint32_t consumer_id);
int sp_subscribe_consumer(uint32_t processor_id, uint32_t consumer_id, uint32_t partition_id);

int sp_consume_events(uint32_t processor_id, uint32_t consumer_id, StreamEvent **events, uint32_t *count);
int sp_commit_offset(uint32_t processor_id, uint32_t consumer_id);
int sp_seek_to_offset(uint32_t processor_id, uint32_t consumer_id, uint64_t offset);

int sp_rebalance_partitions(uint32_t processor_id, uint32_t topic_id);
int sp_migrate_partition(uint32_t processor_id, uint32_t topic_id, uint32_t src_partition, uint32_t dst_partition);

int sp_filter_events(uint32_t processor_id, uint32_t topic_id,
                     int (*filter_fn)(const StreamEvent *));
int sp_map_events(uint32_t processor_id, uint32_t topic_id,
                  int (*map_fn)(StreamEvent *, StreamEvent *));
int sp_aggregate_events(uint32_t processor_id, uint32_t topic_id, uint32_t window_size_ms);

StreamProcessor *sp_get_processor_info(uint32_t processor_id);
StreamTopic *sp_get_topic_info(uint32_t processor_id, uint32_t topic_id);
uint64_t sp_get_topic_lag(uint32_t processor_id, uint32_t consumer_id);
uint32_t sp_get_throughput(uint32_t processor_id);

#endif 
