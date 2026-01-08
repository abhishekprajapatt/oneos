#include "dist_cache.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static DistributedCache g_caches[8] = {0};
static uint32_t g_cache_count = 0;

int dcache_create(const char *name, CacheEvictionPolicy policy, uint32_t max_memory_mb)
{
    if (!name || g_cache_count >= 8)
    {
        return -1;
    }

    DistributedCache *cache = &g_caches[g_cache_count];
    cache->cache_id = g_cache_count + 1;
    strncpy(cache->name, name, 63);
    cache->eviction_policy = policy;
    cache->max_memory_mb = max_memory_mb;
    cache->node_count = 0;
    cache->partition_count = MAX_CACHE_PARTITIONS;
    cache->total_hits = 0;
    cache->total_misses = 0;
    cache->total_bytes_stored = 0;
    cache->enable_replication = 0;

    g_cache_count++;
    return cache->cache_id;
}

int dcache_add_node(uint32_t cache_id, uint32_t ip, uint16_t port)
{
    if (cache_id == 0 || cache_id > g_cache_count)
    {
        return -1;
    }

    DistributedCache *cache = &g_caches[cache_id - 1];
    if (cache->node_count >= MAX_CACHE_NODES)
    {
        return -1;
    }

    CacheNode *node = &cache->nodes[cache->node_count];
    node->node_id = cache->node_count + 1;
    node->ip_address = ip;
    node->port = port;
    node->is_primary = cache->node_count == 0 ? 1 : 0;
    node->is_alive = 1;
    node->last_heartbeat = time(NULL);

    for (uint32_t i = 0; i < MAX_CACHE_PARTITIONS; i++)
    {
        node->partitions[i].partition_id = i;
        node->partitions[i].entry_count = 0;
        node->partitions[i].hits = 0;
        node->partitions[i].misses = 0;
        node->partitions[i].bytes_used = 0;
        node->partitions[i].max_bytes = (cache->max_memory_mb * 1024 * 1024) / MAX_CACHE_PARTITIONS;
    }

    cache->node_count++;
    return node->node_id;
}

int dcache_remove_node(uint32_t cache_id, uint32_t node_id)
{
    if (cache_id == 0 || cache_id > g_cache_count)
    {
        return -1;
    }

    DistributedCache *cache = &g_caches[cache_id - 1];
    if (node_id == 0 || node_id > cache->node_count)
    {
        return -1;
    }

    for (uint32_t i = node_id - 1; i < cache->node_count - 1; i++)
    {
        cache->nodes[i] = cache->nodes[i + 1];
    }
    cache->node_count--;

    return 0;
}

int dcache_promote_replica(uint32_t cache_id, uint32_t node_id)
{
    if (cache_id == 0 || cache_id > g_cache_count)
    {
        return -1;
    }

    DistributedCache *cache = &g_caches[cache_id - 1];
    if (node_id == 0 || node_id > cache->node_count)
    {
        return -1;
    }

    for (uint32_t i = 0; i < cache->node_count; i++)
    {
        cache->nodes[i].is_primary = 0;
    }
    cache->nodes[node_id - 1].is_primary = 1;

    return 0;
}

uint32_t dcache_hash_key(const char *key)
{
    uint32_t hash = 0;
    for (uint32_t i = 0; key[i] != '\0'; i++)
    {
        hash = ((hash << 5) + hash) ^ key[i];
    }
    return hash;
}

int dcache_put(uint32_t cache_id, const char *key, const void *value, size_t value_size, uint32_t ttl_seconds)
{
    if (cache_id == 0 || cache_id > g_cache_count || !key || !value)
    {
        return -1;
    }

    if (value_size > MAX_VALUE_SIZE)
    {
        return -1;
    }

    DistributedCache *cache = &g_caches[cache_id - 1];
    if (cache->node_count == 0)
    {
        return -1;
    }

    uint32_t partition_id = dcache_get_partition_for_key(cache_id, key);
    uint32_t node_id = (dcache_hash_key(key) % cache->node_count);

    CacheNode *node = &cache->nodes[node_id];
    CachePartition *partition = &node->partitions[partition_id];

    for (uint32_t i = 0; i < partition->entry_count; i++)
    {
        if (strcmp(partition->entries[i].key, key) == 0)
        {
            partition->bytes_used -= partition->entries[i].value_size;
            memcpy(partition->entries[i].value, value, value_size);
            partition->entries[i].value_size = value_size;
            partition->entries[i].last_access_time = time(NULL);
            partition->entries[i].ttl_seconds = ttl_seconds;
            partition->entries[i].is_expired = 0;
            partition->bytes_used += value_size;
            return 0;
        }
    }

    if (partition->entry_count >= MAX_CACHE_ENTRIES)
    {
        return -1;
    }

    CacheEntry *entry = &partition->entries[partition->entry_count];
    strncpy(entry->key, key, MAX_KEY_SIZE - 1);
    entry->value = malloc(value_size);
    if (!entry->value)
    {
        return -1;
    }
    memcpy(entry->value, value, value_size);
    entry->value_size = value_size;
    entry->creation_time = time(NULL);
    entry->last_access_time = entry->creation_time;
    entry->access_count = 0;
    entry->ttl_seconds = ttl_seconds;
    entry->is_expired = 0;

    partition->entry_count++;
    partition->bytes_used += value_size;
    cache->total_bytes_stored += value_size;

    return 0;
}

void *dcache_get(uint32_t cache_id, const char *key, size_t *value_size)
{
    if (cache_id == 0 || cache_id > g_cache_count || !key || !value_size)
    {
        return NULL;
    }

    DistributedCache *cache = &g_caches[cache_id - 1];
    if (cache->node_count == 0)
    {
        return NULL;
    }

    uint32_t partition_id = dcache_get_partition_for_key(cache_id, key);
    uint32_t node_id = (dcache_hash_key(key) % cache->node_count);

    CacheNode *node = &cache->nodes[node_id];
    CachePartition *partition = &node->partitions[partition_id];

    for (uint32_t i = 0; i < partition->entry_count; i++)
    {
        CacheEntry *entry = &partition->entries[i];
        if (strcmp(entry->key, key) == 0)
        {
            if (entry->is_expired)
            {
                cache->total_misses++;
                partition->misses++;
                return NULL;
            }

            time_t now = time(NULL);
            if (entry->ttl_seconds > 0 && (now - entry->creation_time) > entry->ttl_seconds)
            {
                entry->is_expired = 1;
                cache->total_misses++;
                partition->misses++;
                return NULL;
            }

            entry->last_access_time = now;
            entry->access_count++;
            *value_size = entry->value_size;
            cache->total_hits++;
            partition->hits++;

            return entry->value;
        }
    }

    cache->total_misses++;
    partition->misses++;
    return NULL;
}

int dcache_delete(uint32_t cache_id, const char *key)
{
    if (cache_id == 0 || cache_id > g_cache_count || !key)
    {
        return -1;
    }

    DistributedCache *cache = &g_caches[cache_id - 1];
    if (cache->node_count == 0)
    {
        return -1;
    }

    uint32_t partition_id = dcache_get_partition_for_key(cache_id, key);
    uint32_t node_id = (dcache_hash_key(key) % cache->node_count);

    CacheNode *node = &cache->nodes[node_id];
    CachePartition *partition = &node->partitions[partition_id];

    for (uint32_t i = 0; i < partition->entry_count; i++)
    {
        if (strcmp(partition->entries[i].key, key) == 0)
        {
            cache->total_bytes_stored -= partition->entries[i].value_size;
            partition->bytes_used -= partition->entries[i].value_size;
            free(partition->entries[i].value);

            for (uint32_t j = i; j < partition->entry_count - 1; j++)
            {
                partition->entries[j] = partition->entries[j + 1];
            }
            partition->entry_count--;
            return 0;
        }
    }

    return -1;
}

int dcache_exists(uint32_t cache_id, const char *key)
{
    if (cache_id == 0 || cache_id > g_cache_count || !key)
    {
        return 0;
    }

    size_t dummy;
    return dcache_get(cache_id, key, &dummy) != NULL ? 1 : 0;
}

int dcache_enable_replication(uint32_t cache_id, uint8_t num_replicas)
{
    if (cache_id == 0 || cache_id > g_cache_count)
    {
        return -1;
    }

    DistributedCache *cache = &g_caches[cache_id - 1];
    if (num_replicas > cache->node_count)
    {
        return -1;
    }

    cache->enable_replication = 1;
    return 0;
}

int dcache_sync_node(uint32_t cache_id, uint32_t node_id)
{
    if (cache_id == 0 || cache_id > g_cache_count)
    {
        return -1;
    }

    DistributedCache *cache = &g_caches[cache_id - 1];
    if (node_id == 0 || node_id > cache->node_count)
    {
        return -1;
    }

    cache->nodes[node_id - 1].last_heartbeat = time(NULL);
    return 0;
}

int dcache_invalidate_key(uint32_t cache_id, const char *key)
{
    return dcache_delete(cache_id, key);
}

int dcache_invalidate_pattern(uint32_t cache_id, const char *pattern)
{
    if (cache_id == 0 || cache_id > g_cache_count || !pattern)
    {
        return -1;
    }

    return 0;
}

int dcache_update_ttl(uint32_t cache_id, const char *key, uint32_t ttl_seconds)
{
    if (cache_id == 0 || cache_id > g_cache_count || !key)
    {
        return -1;
    }

    DistributedCache *cache = &g_caches[cache_id - 1];
    if (cache->node_count == 0)
    {
        return -1;
    }

    uint32_t partition_id = dcache_get_partition_for_key(cache_id, key);
    uint32_t node_id = (dcache_hash_key(key) % cache->node_count);

    CacheNode *node = &cache->nodes[node_id];
    CachePartition *partition = &node->partitions[partition_id];

    for (uint32_t i = 0; i < partition->entry_count; i++)
    {
        if (strcmp(partition->entries[i].key, key) == 0)
        {
            partition->entries[i].ttl_seconds = ttl_seconds;
            return 0;
        }
    }

    return -1;
}

int dcache_expire_old_entries(uint32_t cache_id)
{
    if (cache_id == 0 || cache_id > g_cache_count)
    {
        return -1;
    }

    DistributedCache *cache = &g_caches[cache_id - 1];
    time_t now = time(NULL);
    int expired_count = 0;

    for (uint32_t n = 0; n < cache->node_count; n++)
    {
        for (uint32_t p = 0; p < MAX_CACHE_PARTITIONS; p++)
        {
            CachePartition *partition = &cache->nodes[n].partitions[p];
            for (uint32_t i = 0; i < partition->entry_count; i++)
            {
                CacheEntry *entry = &partition->entries[i];
                if (entry->ttl_seconds > 0 &&
                    (now - entry->creation_time) > entry->ttl_seconds)
                {
                    entry->is_expired = 1;
                    expired_count++;
                }
            }
        }
    }

    return expired_count;
}

int dcache_set_global_ttl(uint32_t cache_id, uint32_t ttl_seconds)
{
    if (cache_id == 0 || cache_id > g_cache_count)
    {
        return -1;
    }

    return 0;
}

uint32_t dcache_get_partition_for_key(uint32_t cache_id, const char *key)
{
    if (cache_id == 0 || cache_id > g_cache_count || !key)
    {
        return 0;
    }

    return dcache_hash_key(key) % MAX_CACHE_PARTITIONS;
}

int dcache_rebalance_partitions(uint32_t cache_id)
{
    if (cache_id == 0 || cache_id > g_cache_count)
    {
        return -1;
    }

    return 0;
}

int dcache_migrate_partition(uint32_t cache_id, uint32_t src_node, uint32_t dst_node, uint32_t partition_id)
{
    if (cache_id == 0 || cache_id > g_cache_count)
    {
        return -1;
    }

    return 0;
}

DistributedCache *dcache_get_info(uint32_t cache_id)
{
    if (cache_id == 0 || cache_id > g_cache_count)
    {
        return NULL;
    }

    return &g_caches[cache_id - 1];
}

uint64_t dcache_get_hit_rate(uint32_t cache_id)
{
    if (cache_id == 0 || cache_id > g_cache_count)
    {
        return 0;
    }

    DistributedCache *cache = &g_caches[cache_id - 1];
    uint64_t total = cache->total_hits + cache->total_misses;
    if (total == 0)
        return 0;

    return (cache->total_hits * 100) / total;
}

uint64_t dcache_get_memory_usage(uint32_t cache_id)
{
    if (cache_id == 0 || cache_id > g_cache_count)
    {
        return 0;
    }

    return g_caches[cache_id - 1].total_bytes_stored;
}

int dcache_flush_all(uint32_t cache_id)
{
    if (cache_id == 0 || cache_id > g_cache_count)
    {
        return -1;
    }

    DistributedCache *cache = &g_caches[cache_id - 1];
    for (uint32_t n = 0; n < cache->node_count; n++)
    {
        for (uint32_t p = 0; p < MAX_CACHE_PARTITIONS; p++)
        {
            CachePartition *partition = &cache->nodes[n].partitions[p];
            for (uint32_t i = 0; i < partition->entry_count; i++)
            {
                free(partition->entries[i].value);
            }
            partition->entry_count = 0;
            partition->bytes_used = 0;
            partition->hits = 0;
            partition->misses = 0;
        }
    }

    cache->total_bytes_stored = 0;
    cache->total_hits = 0;
    cache->total_misses = 0;

    return 0;
}

int dcache_cleanup_expired(uint32_t cache_id)
{
    if (cache_id == 0 || cache_id > g_cache_count)
    {
        return -1;
    }

    DistributedCache *cache = &g_caches[cache_id - 1];
    time_t now = time(NULL);
    int cleaned = 0;

    for (uint32_t n = 0; n < cache->node_count; n++)
    {
        for (uint32_t p = 0; p < MAX_CACHE_PARTITIONS; p++)
        {
            CachePartition *partition = &cache->nodes[n].partitions[p];
            uint32_t write_idx = 0;

            for (uint32_t i = 0; i < partition->entry_count; i++)
            {
                CacheEntry *entry = &partition->entries[i];
                uint32_t age = now - entry->creation_time;

                if (entry->is_expired || (entry->ttl_seconds > 0 && age > entry->ttl_seconds))
                {
                    cache->total_bytes_stored -= entry->value_size;
                    partition->bytes_used -= entry->value_size;
                    free(entry->value);
                    cleaned++;
                }
                else
                {
                    partition->entries[write_idx++] = partition->entries[i];
                }
            }
            partition->entry_count = write_idx;
        }
    }

    return cleaned;
}
