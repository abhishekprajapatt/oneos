#ifndef DIST_CACHE_H
#define DIST_CACHE_H

#include <stdint.h>
#include <stddef.h>
#include <time.h>

#define MAX_CACHE_ENTRIES 8192
#define MAX_CACHE_NODES 64
#define MAX_KEY_SIZE 256
#define MAX_VALUE_SIZE 65536
#define MAX_CACHE_PARTITIONS 16

typedef enum
{
    CACHE_LRU = 0,
    CACHE_LFU = 1,
    CACHE_ARC = 2,
    CACHE_FIFO = 3
} CacheEvictionPolicy;

typedef enum
{
    CACHE_READ = 0,
    CACHE_WRITE = 1,
    CACHE_DELETE = 2
} CacheOperation;

typedef struct
{
    char key[MAX_KEY_SIZE];
    void *value;
    size_t value_size;
    uint64_t creation_time;
    uint64_t last_access_time;
    uint32_t access_count;
    uint32_t ttl_seconds;
    uint8_t is_expired;
} CacheEntry;

typedef struct
{
    uint32_t partition_id;
    uint32_t entry_count;
    CacheEntry entries[MAX_CACHE_ENTRIES];
    uint64_t hits;
    uint64_t misses;
    uint64_t bytes_used;
    uint32_t max_bytes;
} CachePartition;

typedef struct
{
    uint32_t node_id;
    uint32_t ip_address;
    uint16_t port;
    uint8_t is_primary;
    uint8_t is_alive;
    uint64_t last_heartbeat;
    CachePartition partitions[MAX_CACHE_PARTITIONS];
} CacheNode;

typedef struct
{
    uint32_t cache_id;
    char name[64];
    uint32_t node_count;
    CacheNode nodes[MAX_CACHE_NODES];
    CacheEvictionPolicy eviction_policy;
    uint32_t max_memory_mb;
    uint32_t partition_count;
    uint64_t total_hits;
    uint64_t total_misses;
    uint64_t total_bytes_stored;
    uint8_t enable_replication;
} DistributedCache;

// Cache initialization and management
int dcache_create(const char *name, CacheEvictionPolicy policy, uint32_t max_memory_mb);
int dcache_add_node(uint32_t cache_id, uint32_t ip, uint16_t port);
int dcache_remove_node(uint32_t cache_id, uint32_t node_id);
int dcache_promote_replica(uint32_t cache_id, uint32_t node_id);

// Core cache operations
int dcache_put(uint32_t cache_id, const char *key, const void *value, size_t value_size, uint32_t ttl_seconds);
void *dcache_get(uint32_t cache_id, const char *key, size_t *value_size);
int dcache_delete(uint32_t cache_id, const char *key);
int dcache_exists(uint32_t cache_id, const char *key);

// Replication and consistency
int dcache_enable_replication(uint32_t cache_id, uint8_t num_replicas);
int dcache_sync_node(uint32_t cache_id, uint32_t node_id);
int dcache_invalidate_key(uint32_t cache_id, const char *key);
int dcache_invalidate_pattern(uint32_t cache_id, const char *pattern);

// TTL and expiration
int dcache_update_ttl(uint32_t cache_id, const char *key, uint32_t ttl_seconds);
int dcache_expire_old_entries(uint32_t cache_id);
int dcache_set_global_ttl(uint32_t cache_id, uint32_t ttl_seconds);

// Partitioning and sharding
uint32_t dcache_get_partition_for_key(uint32_t cache_id, const char *key);
int dcache_rebalance_partitions(uint32_t cache_id);
int dcache_migrate_partition(uint32_t cache_id, uint32_t src_node, uint32_t dst_node, uint32_t partition_id);

// Statistics and monitoring
DistributedCache *dcache_get_info(uint32_t cache_id);
uint64_t dcache_get_hit_rate(uint32_t cache_id);
uint64_t dcache_get_memory_usage(uint32_t cache_id);
int dcache_flush_all(uint32_t cache_id);
int dcache_cleanup_expired(uint32_t cache_id);

#endif // DIST_CACHE_H
