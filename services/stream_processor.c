#include "stream_processor.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static StreamProcessor g_processors[16] = {0};
static uint32_t g_processor_count = 0;

uint32_t sp_hash_key(const char *key)
{
    uint32_t hash = 0;
    for (uint32_t i = 0; key[i] != '\0'; i++)
    {
        hash = ((hash << 5) + hash) ^ key[i];
    }
    return hash;
}

int sp_create_processor(const char *name)
{
    if (!name || g_processor_count >= 16)
    {
        return -1;
    }

    StreamProcessor *proc = &g_processors[g_processor_count];
    proc->processor_id = g_processor_count + 1;
    strncpy(proc->name, name, 63);
    proc->topic_count = 0;
    proc->consumer_count = 0;
    proc->total_events_processed = 0;
    proc->total_latency_ms = 0;
    proc->peak_throughput_events_sec = 0;

    g_processor_count++;
    return proc->processor_id;
}

int sp_delete_processor(uint32_t processor_id)
{
    if (processor_id == 0 || processor_id > g_processor_count)
    {
        return -1;
    }

    StreamProcessor *proc = &g_processors[processor_id - 1];

    for (uint32_t i = 0; i < proc->topic_count; i++)
    {
        for (uint32_t j = 0; j < proc->topics[i].partition_count; j++)
        {
            if (proc->topics[i].partitions[j].events)
            {
                free(proc->topics[i].partitions[j].events);
            }
        }
    }

    for (uint32_t i = processor_id - 1; i < g_processor_count - 1; i++)
    {
        g_processors[i] = g_processors[i + 1];
    }
    g_processor_count--;

    return 0;
}

int sp_create_topic(uint32_t processor_id, const char *topic_name, uint32_t partition_count)
{
    if (processor_id == 0 || processor_id > g_processor_count || !topic_name)
    {
        return -1;
    }

    if (partition_count > MAX_STREAM_PARTITIONS || partition_count == 0)
    {
        return -1;
    }

    StreamProcessor *proc = &g_processors[processor_id - 1];
    if (proc->topic_count >= MAX_TOPICS)
    {
        return -1;
    }

    StreamTopic *topic = &proc->topics[proc->topic_count];
    topic->topic_id = proc->topic_count + 1;
    strncpy(topic->topic_name, topic_name, 63);
    topic->partition_count = partition_count;
    topic->partition_strategy = PARTITION_STRATEGY_KEY_HASH;
    topic->total_events = 0;
    topic->total_bytes = 0;
    topic->consumer_count = 0;
    topic->retention_policy = 1;
    topic->retention_hours = 24;

    for (uint32_t i = 0; i < partition_count; i++)
    {
        StreamPartition *partition = &topic->partitions[i];
        partition->partition_id = i;
        partition->event_count = 0;
        partition->offset = 0;
        partition->capacity = MAX_STREAM_BUFFER / partition_count;
        partition->events = malloc(partition->capacity * sizeof(StreamEvent));

        if (!partition->events)
        {
            return -1;
        }
    }

    proc->topic_count++;
    return topic->topic_id;
}

int sp_delete_topic(uint32_t processor_id, uint32_t topic_id)
{
    if (processor_id == 0 || processor_id > g_processor_count)
    {
        return -1;
    }

    StreamProcessor *proc = &g_processors[processor_id - 1];
    if (topic_id == 0 || topic_id > proc->topic_count)
    {
        return -1;
    }

    StreamTopic *topic = &proc->topics[topic_id - 1];
    for (uint32_t i = 0; i < topic->partition_count; i++)
    {
        if (topic->partitions[i].events)
        {
            free(topic->partitions[i].events);
        }
    }

    for (uint32_t i = topic_id - 1; i < proc->topic_count - 1; i++)
    {
        proc->topics[i] = proc->topics[i + 1];
    }
    proc->topic_count--;

    return 0;
}

int sp_set_topic_retention(uint32_t processor_id, uint32_t topic_id, uint32_t hours)
{
    if (processor_id == 0 || processor_id > g_processor_count)
    {
        return -1;
    }

    StreamProcessor *proc = &g_processors[processor_id - 1];
    if (topic_id == 0 || topic_id > proc->topic_count)
    {
        return -1;
    }

    proc->topics[topic_id - 1].retention_hours = hours;
    return 0;
}

int sp_publish_event(uint32_t processor_id, uint32_t topic_id, const void *payload, uint32_t payload_size)
{
    if (processor_id == 0 || processor_id > g_processor_count || !payload)
    {
        return -1;
    }

    StreamProcessor *proc = &g_processors[processor_id - 1];
    if (topic_id == 0 || topic_id > proc->topic_count)
    {
        return -1;
    }

    StreamTopic *topic = &proc->topics[topic_id - 1];
    uint32_t partition_id = (rand() % topic->partition_count);

    StreamPartition *partition = &topic->partitions[partition_id];
    if (partition->event_count >= partition->capacity)
    {
        return -1;
    }

    StreamEvent *event = &partition->events[partition->event_count];
    event->event_id = topic->total_events + 1;
    event->timestamp = time(NULL);
    event->key_hash = 0;
    event->payload = malloc(payload_size);

    if (!event->payload)
    {
        return -1;
    }

    memcpy(event->payload, payload, payload_size);
    event->payload_size = payload_size;
    event->is_processed = 0;
    event->partition_id = partition_id;

    partition->event_count++;
    topic->total_events++;
    topic->total_bytes += payload_size;
    topic->last_event_time = time(NULL);

    proc->total_events_processed++;

    return event->event_id;
}

int sp_publish_with_key(uint32_t processor_id, uint32_t topic_id, const char *key, const void *payload, uint32_t payload_size)
{
    if (processor_id == 0 || processor_id > g_processor_count || !key || !payload)
    {
        return -1;
    }

    StreamProcessor *proc = &g_processors[processor_id - 1];
    if (topic_id == 0 || topic_id > proc->topic_count)
    {
        return -1;
    }

    StreamTopic *topic = &proc->topics[topic_id - 1];
    uint32_t partition_id = (sp_hash_key(key) % topic->partition_count);

    StreamPartition *partition = &topic->partitions[partition_id];
    if (partition->event_count >= partition->capacity)
    {
        return -1;
    }

    StreamEvent *event = &partition->events[partition->event_count];
    event->event_id = topic->total_events + 1;
    event->timestamp = time(NULL);
    event->key_hash = sp_hash_key(key);
    event->payload = malloc(payload_size);

    if (!event->payload)
    {
        return -1;
    }

    memcpy(event->payload, payload, payload_size);
    event->payload_size = payload_size;
    event->is_processed = 0;
    event->partition_id = partition_id;

    partition->event_count++;
    topic->total_events++;
    topic->total_bytes += payload_size;
    topic->last_event_time = time(NULL);

    return event->event_id;
}

int sp_publish_batch(uint32_t processor_id, uint32_t topic_id, const void **payloads, const uint32_t *sizes, uint32_t count)
{
    if (processor_id == 0 || processor_id > g_processor_count || !payloads || !sizes)
    {
        return -1;
    }

    int published_count = 0;
    for (uint32_t i = 0; i < count; i++)
    {
        if (sp_publish_event(processor_id, topic_id, payloads[i], sizes[i]) >= 0)
        {
            published_count++;
        }
    }

    return published_count;
}

int sp_create_consumer(uint32_t processor_id, uint32_t topic_id, const char *consumer_group, uint32_t batch_size)
{
    if (processor_id == 0 || processor_id > g_processor_count || !consumer_group)
    {
        return -1;
    }

    StreamProcessor *proc = &g_processors[processor_id - 1];
    if (topic_id == 0 || topic_id > proc->topic_count)
    {
        return -1;
    }

    if (proc->consumer_count >= MAX_CONSUMERS)
    {
        return -1;
    }

    StreamConsumer *consumer = &proc->consumers[proc->consumer_count];
    consumer->consumer_id = proc->consumer_count + 1;
    strncpy(consumer->consumer_group, consumer_group, 63);
    consumer->topic_id = topic_id;
    consumer->current_partition = 0;
    consumer->current_offset = 0;
    consumer->last_commit_offset = 0;
    consumer->batch_size = batch_size > 0 ? batch_size : 1;
    consumer->processed_events = 0;
    consumer->processed_bytes = 0;
    consumer->is_active = 1;

    proc->topics[topic_id - 1].consumer_count++;
    proc->consumer_count++;

    return consumer->consumer_id;
}

int sp_delete_consumer(uint32_t processor_id, uint32_t consumer_id)
{
    if (processor_id == 0 || processor_id > g_processor_count)
    {
        return -1;
    }

    StreamProcessor *proc = &g_processors[processor_id - 1];
    if (consumer_id == 0 || consumer_id > proc->consumer_count)
    {
        return -1;
    }

    for (uint32_t i = consumer_id - 1; i < proc->consumer_count - 1; i++)
    {
        proc->consumers[i] = proc->consumers[i + 1];
    }
    proc->consumer_count--;

    return 0;
}

int sp_subscribe_consumer(uint32_t processor_id, uint32_t consumer_id, uint32_t partition_id)
{
    if (processor_id == 0 || processor_id > g_processor_count)
    {
        return -1;
    }

    StreamProcessor *proc = &g_processors[processor_id - 1];
    if (consumer_id == 0 || consumer_id > proc->consumer_count)
    {
        return -1;
    }

    StreamConsumer *consumer = &proc->consumers[consumer_id - 1];
    consumer->current_partition = partition_id;
    consumer->current_offset = 0;

    return 0;
}

int sp_consume_events(uint32_t processor_id, uint32_t consumer_id, StreamEvent **events, uint32_t *count)
{
    if (processor_id == 0 || processor_id > g_processor_count || !events || !count)
    {
        return -1;
    }

    StreamProcessor *proc = &g_processors[processor_id - 1];
    if (consumer_id == 0 || consumer_id > proc->consumer_count)
    {
        return -1;
    }

    StreamConsumer *consumer = &proc->consumers[consumer_id - 1];
    StreamTopic *topic = &proc->topics[consumer->topic_id - 1];

    if (consumer->current_partition >= topic->partition_count)
    {
        *count = 0;
        return 0;
    }

    StreamPartition *partition = &topic->partitions[consumer->current_partition];
    uint32_t available = partition->event_count - consumer->current_offset;
    uint32_t to_consume = available < consumer->batch_size ? available : consumer->batch_size;

    *events = &partition->events[consumer->current_offset];
    *count = to_consume;

    consumer->current_offset += to_consume;
    consumer->processed_events += to_consume;

    return to_consume;
}

int sp_commit_offset(uint32_t processor_id, uint32_t consumer_id)
{
    if (processor_id == 0 || processor_id > g_processor_count)
    {
        return -1;
    }

    StreamProcessor *proc = &g_processors[processor_id - 1];
    if (consumer_id == 0 || consumer_id > proc->consumer_count)
    {
        return -1;
    }

    StreamConsumer *consumer = &proc->consumers[consumer_id - 1];
    consumer->last_commit_offset = consumer->current_offset;

    return 0;
}

int sp_seek_to_offset(uint32_t processor_id, uint32_t consumer_id, uint64_t offset)
{
    if (processor_id == 0 || processor_id > g_processor_count)
    {
        return -1;
    }

    StreamProcessor *proc = &g_processors[processor_id - 1];
    if (consumer_id == 0 || consumer_id > proc->consumer_count)
    {
        return -1;
    }

    StreamConsumer *consumer = &proc->consumers[consumer_id - 1];
    consumer->current_offset = offset;

    return 0;
}

int sp_rebalance_partitions(uint32_t processor_id, uint32_t topic_id)
{
    if (processor_id == 0 || processor_id > g_processor_count)
    {
        return -1;
    }

    return 0;
}

int sp_migrate_partition(uint32_t processor_id, uint32_t topic_id, uint32_t src_partition, uint32_t dst_partition)
{
    if (processor_id == 0 || processor_id > g_processor_count)
    {
        return -1;
    }

    return 0;
}

int sp_filter_events(uint32_t processor_id, uint32_t topic_id,
                     int (*filter_fn)(const StreamEvent *))
{
    if (processor_id == 0 || processor_id > g_processor_count || !filter_fn)
    {
        return -1;
    }

    return 0;
}

int sp_map_events(uint32_t processor_id, uint32_t topic_id,
                  int (*map_fn)(StreamEvent *, StreamEvent *))
{
    if (processor_id == 0 || processor_id > g_processor_count || !map_fn)
    {
        return -1;
    }

    return 0;
}

int sp_aggregate_events(uint32_t processor_id, uint32_t topic_id, uint32_t window_size_ms)
{
    if (processor_id == 0 || processor_id > g_processor_count)
    {
        return -1;
    }

    return 0;
}

StreamProcessor *sp_get_processor_info(uint32_t processor_id)
{
    if (processor_id == 0 || processor_id > g_processor_count)
    {
        return NULL;
    }

    return &g_processors[processor_id - 1];
}

StreamTopic *sp_get_topic_info(uint32_t processor_id, uint32_t topic_id)
{
    if (processor_id == 0 || processor_id > g_processor_count)
    {
        return NULL;
    }

    StreamProcessor *proc = &g_processors[processor_id - 1];
    if (topic_id == 0 || topic_id > proc->topic_count)
    {
        return NULL;
    }

    return &proc->topics[topic_id - 1];
}

uint64_t sp_get_topic_lag(uint32_t processor_id, uint32_t consumer_id)
{
    if (processor_id == 0 || processor_id > g_processor_count)
    {
        return 0;
    }

    StreamProcessor *proc = &g_processors[processor_id - 1];
    if (consumer_id == 0 || consumer_id > proc->consumer_count)
    {
        return 0;
    }

    StreamConsumer *consumer = &proc->consumers[consumer_id - 1];
    StreamTopic *topic = &proc->topics[consumer->topic_id - 1];
    StreamPartition *partition = &topic->partitions[consumer->current_partition];

    return (partition->event_count - consumer->current_offset);
}

uint32_t sp_get_throughput(uint32_t processor_id)
{
    if (processor_id == 0 || processor_id > g_processor_count)
    {
        return 0;
    }

    return g_processors[processor_id - 1].peak_throughput_events_sec;
}
