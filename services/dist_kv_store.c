#include "dist_kv_store.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static DistributedKVStore g_stores[MAX_KV_STORES] = {0};
static uint32_t g_store_count = 0;
static uint64_t g_tx_id_counter = 1;

static uint32_t hash_key(const char *key)
{
    uint32_t hash = 5381;
    int c;
    while ((c = *key++))
    {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

static int find_entry(DistributedKVStore *store, const char *key, uint32_t *idx)
{
    for (uint32_t i = 0; i < store->entry_count; i++)
    {
        if (strcmp(store->entries[i].key, key) == 0)
        {
            *idx = i;
            return 0;
        }
    }
    return -1;
}

int kv_create_store(const char *name, ConsistencyModel consistency, uint32_t replica_factor)
{
    if (!name || g_store_count >= MAX_KV_STORES)
    {
        return -1;
    }

    DistributedKVStore *store = &g_stores[g_store_count];
    store->store_id = g_store_count + 1;
    strncpy(store->name, name, 63);
    store->consistency = consistency;
    store->node_count = 0;
    store->entry_count = 0;
    store->tx_count = 0;
    store->replica_factor = (replica_factor > MAX_REPLICAS) ? MAX_REPLICAS : replica_factor;
    store->quorum_size = (replica_factor / 2) + 1;
    store->hash_ring_size = 160;

    store->hash_ring = (uint32_t *)malloc(store->hash_ring_size * sizeof(uint32_t));
    if (!store->hash_ring)
    {
        return -1;
    }

    memset(store->hash_ring, 0, store->hash_ring_size * sizeof(uint32_t));

    store->total_reads = 0;
    store->total_writes = 0;
    store->total_deletes = 0;
    store->bytes_total = 0;

    g_store_count++;
    return store->store_id;
}

int kv_delete_store(uint32_t store_id)
{
    if (store_id == 0 || store_id > g_store_count)
    {
        return -1;
    }

    DistributedKVStore *store = &g_stores[store_id - 1];
    if (store->hash_ring)
    {
        free(store->hash_ring);
    }

    for (uint32_t i = store_id - 1; i < g_store_count - 1; i++)
    {
        g_stores[i] = g_stores[i + 1];
    }
    g_store_count--;

    return 0;
}

int kv_add_node(uint32_t store_id, uint32_t node_id, const char *address, uint16_t port)
{
    if (store_id == 0 || store_id > g_store_count || !address)
    {
        return -1;
    }

    DistributedKVStore *store = &g_stores[store_id - 1];
    if (store->node_count >= MAX_NODES)
    {
        return -1;
    }

    Node *node = &store->nodes[store->node_count];
    node->node_id = node_id;
    strncpy(node->address, address, 63);
    node->port = port;
    node->health = NODE_HEALTHY;
    node->last_heartbeat = 0;
    node->replication_lag = 0;
    node->keys_stored = 0;
    node->bytes_stored = 0;

    store->node_count++;

    return 0;
}

int kv_remove_node(uint32_t store_id, uint32_t node_id)
{
    if (store_id == 0 || store_id > g_store_count)
    {
        return -1;
    }

    DistributedKVStore *store = &g_stores[store_id - 1];

    for (uint32_t i = 0; i < store->node_count; i++)
    {
        if (store->nodes[i].node_id == node_id)
        {
            for (uint32_t j = i; j < store->node_count - 1; j++)
            {
                store->nodes[j] = store->nodes[j + 1];
            }
            store->node_count--;
            return 0;
        }
    }

    return -1;
}

int kv_get_node_health(uint32_t store_id, uint32_t node_id)
{
    if (store_id == 0 || store_id > g_store_count)
    {
        return -1;
    }

    DistributedKVStore *store = &g_stores[store_id - 1];

    for (uint32_t i = 0; i < store->node_count; i++)
    {
        if (store->nodes[i].node_id == node_id)
        {
            return store->nodes[i].health;
        }
    }

    return -1;
}

int kv_update_node_health(uint32_t store_id, uint32_t node_id, NodeHealth health)
{
    if (store_id == 0 || store_id > g_store_count)
    {
        return -1;
    }

    DistributedKVStore *store = &g_stores[store_id - 1];

    for (uint32_t i = 0; i < store->node_count; i++)
    {
        if (store->nodes[i].node_id == node_id)
        {
            store->nodes[i].health = health;
            return 0;
        }
    }

    return -1;
}

int kv_put(uint32_t store_id, const char *key, const char *value, uint32_t value_len)
{
    if (store_id == 0 || store_id > g_store_count || !key || !value)
    {
        return -1;
    }

    if (value_len > MAX_VALUE_SIZE)
    {
        return -1;
    }

    DistributedKVStore *store = &g_stores[store_id - 1];
    uint32_t entry_idx = 0;

    if (find_entry(store, key, &entry_idx) == 0)
    {
        KeyValueEntry *entry = &store->entries[entry_idx];

        if (entry->version_count < MAX_VERSIONS)
        {
            ValueVersion *new_version = &entry->versions[entry->version_count];
            new_version->version = entry->version_count + 1;
            new_version->timestamp = 0;
            memcpy(new_version->value, value, value_len);
            new_version->value_len = value_len;
            strncpy(new_version->author, "system", 63);

            entry->version_count++;
            entry->current_version = new_version->version;
            entry->last_modified = 0;
            entry->is_tombstone = 0;
        }
    }
    else
    {
        if (store->entry_count >= MAX_KEYS)
        {
            return -1;
        }

        KeyValueEntry *entry = &store->entries[store->entry_count];
        strncpy(entry->key, key, MAX_KEY_SIZE - 1);

        ValueVersion *first_version = &entry->versions[0];
        first_version->version = 1;
        first_version->timestamp = 0;
        memcpy(first_version->value, value, value_len);
        first_version->value_len = value_len;
        strncpy(first_version->author, "system", 63);

        entry->version_count = 1;
        entry->current_version = 1;
        entry->created_at = 0;
        entry->last_modified = 0;
        entry->access_count = 0;
        entry->is_tombstone = 0;

        store->entry_count++;
    }

    store->total_writes++;
    store->bytes_total += value_len;

    return 0;
}

int kv_get(uint32_t store_id, const char *key, char *value, uint32_t *value_len)
{
    if (store_id == 0 || store_id > g_store_count || !key || !value || !value_len)
    {
        return -1;
    }

    DistributedKVStore *store = &g_stores[store_id - 1];
    uint32_t entry_idx = 0;

    if (find_entry(store, key, &entry_idx) != 0)
    {
        return -1;
    }

    KeyValueEntry *entry = &store->entries[entry_idx];
    if (entry->is_tombstone)
    {
        return -1;
    }

    ValueVersion *current = &entry->versions[entry->current_version - 1];
    if (current->value_len > *value_len)
    {
        return -1;
    }

    memcpy(value, current->value, current->value_len);
    *value_len = current->value_len;
    entry->access_count++;
    store->total_reads++;

    return 0;
}

int kv_delete(uint32_t store_id, const char *key)
{
    if (store_id == 0 || store_id > g_store_count || !key)
    {
        return -1;
    }

    DistributedKVStore *store = &g_stores[store_id - 1];
    uint32_t entry_idx = 0;

    if (find_entry(store, key, &entry_idx) != 0)
    {
        return -1;
    }

    store->entries[entry_idx].is_tombstone = 1;
    store->total_deletes++;

    return 0;
}

int kv_exists(uint32_t store_id, const char *key)
{
    if (store_id == 0 || store_id > g_store_count || !key)
    {
        return -1;
    }

    DistributedKVStore *store = &g_stores[store_id - 1];
    uint32_t entry_idx = 0;

    if (find_entry(store, key, &entry_idx) != 0)
    {
        return 0;
    }

    return store->entries[entry_idx].is_tombstone ? 0 : 1;
}

int kv_get_version(uint32_t store_id, const char *key, uint32_t version, char *value, uint32_t *value_len)
{
    if (store_id == 0 || store_id > g_store_count || !key || !value || !value_len)
    {
        return -1;
    }

    DistributedKVStore *store = &g_stores[store_id - 1];
    uint32_t entry_idx = 0;

    if (find_entry(store, key, &entry_idx) != 0)
    {
        return -1;
    }

    KeyValueEntry *entry = &store->entries[entry_idx];
    if (version == 0 || version > entry->version_count)
    {
        return -1;
    }

    ValueVersion *ver = &entry->versions[version - 1];
    if (ver->value_len > *value_len)
    {
        return -1;
    }

    memcpy(value, ver->value, ver->value_len);
    *value_len = ver->value_len;

    return 0;
}

int kv_get_all_versions(uint32_t store_id, const char *key, char **values, uint32_t *value_lens, uint32_t *version_count)
{
    if (store_id == 0 || store_id > g_store_count || !key || !version_count)
    {
        return -1;
    }

    DistributedKVStore *store = &g_stores[store_id - 1];
    uint32_t entry_idx = 0;

    if (find_entry(store, key, &entry_idx) != 0)
    {
        return -1;
    }

    KeyValueEntry *entry = &store->entries[entry_idx];
    if (entry->version_count > *version_count)
    {
        return -1;
    }

    for (uint32_t i = 0; i < entry->version_count; i++)
    {
        if (values && values[i])
        {
            memcpy(values[i], entry->versions[i].value, entry->versions[i].value_len);
            if (value_lens)
            {
                value_lens[i] = entry->versions[i].value_len;
            }
        }
    }

    *version_count = entry->version_count;
    return 0;
}

int kv_get_version_history(uint32_t store_id, const char *key, ValueVersion *versions, uint32_t *count)
{
    if (store_id == 0 || store_id > g_store_count || !key || !versions || !count)
    {
        return -1;
    }

    DistributedKVStore *store = &g_stores[store_id - 1];
    uint32_t entry_idx = 0;

    if (find_entry(store, key, &entry_idx) != 0)
    {
        return -1;
    }

    KeyValueEntry *entry = &store->entries[entry_idx];
    if (entry->version_count > *count)
    {
        return -1;
    }

    memcpy(versions, entry->versions, entry->version_count * sizeof(ValueVersion));
    *count = entry->version_count;

    return 0;
}

uint64_t kv_start_transaction(uint32_t store_id, const char *operation, const char *key, const char *value, uint32_t value_len)
{
    if (store_id == 0 || store_id > g_store_count || !operation || !key)
    {
        return 0;
    }

    DistributedKVStore *store = &g_stores[store_id - 1];
    if (store->tx_count >= MAX_TRANSACTIONS)
    {
        return 0;
    }

    Transaction *tx = &store->transactions[store->tx_count];
    tx->tx_id = g_tx_id_counter++;
    strncpy(tx->operation, operation, 31);
    strncpy(tx->key, key, MAX_KEY_SIZE - 1);

    if (value && value_len > 0)
    {
        memcpy(tx->value, value, (value_len > MAX_VALUE_SIZE) ? MAX_VALUE_SIZE : value_len);
        tx->value_len = (value_len > MAX_VALUE_SIZE) ? MAX_VALUE_SIZE : value_len;
    }
    else
    {
        tx->value_len = 0;
    }

    tx->state = TX_PENDING;
    tx->start_time = 0;
    tx->commit_time = 0;
    tx->read_version = 0;
    tx->write_version = 0;

    store->tx_count++;

    return tx->tx_id;
}

int kv_commit_transaction(uint32_t store_id, uint64_t tx_id)
{
    if (store_id == 0 || store_id > g_store_count)
    {
        return -1;
    }

    DistributedKVStore *store = &g_stores[store_id - 1];

    for (uint32_t i = 0; i < store->tx_count; i++)
    {
        if (store->transactions[i].tx_id == tx_id)
        {
            Transaction *tx = &store->transactions[i];

            if (strcmp(tx->operation, "PUT") == 0)
            {
                kv_put(store_id, tx->key, tx->value, tx->value_len);
            }
            else if (strcmp(tx->operation, "DELETE") == 0)
            {
                kv_delete(store_id, tx->key);
            }

            tx->state = TX_COMMITTED;
            tx->commit_time = 0;
            return 0;
        }
    }

    return -1;
}

int kv_abort_transaction(uint32_t store_id, uint64_t tx_id)
{
    if (store_id == 0 || store_id > g_store_count)
    {
        return -1;
    }

    DistributedKVStore *store = &g_stores[store_id - 1];

    for (uint32_t i = 0; i < store->tx_count; i++)
    {
        if (store->transactions[i].tx_id == tx_id)
        {
            store->transactions[i].state = TX_ABORTED;
            return 0;
        }
    }

    return -1;
}

int kv_get_transaction_state(uint32_t store_id, uint64_t tx_id, TransactionState *state)
{
    if (store_id == 0 || store_id > g_store_count || !state)
    {
        return -1;
    }

    DistributedKVStore *store = &g_stores[store_id - 1];

    for (uint32_t i = 0; i < store->tx_count; i++)
    {
        if (store->transactions[i].tx_id == tx_id)
        {
            *state = store->transactions[i].state;
            return 0;
        }
    }

    return -1;
}

int kv_get_replicas(uint32_t store_id, const char *key, ReplicaNode *replicas, uint32_t *count)
{
    if (store_id == 0 || store_id > g_store_count || !key || !replicas || !count)
    {
        return -1;
    }

    DistributedKVStore *store = &g_stores[store_id - 1];
    if (store->node_count == 0)
    {
        return -1;
    }

    uint32_t key_hash = hash_key(key);
    uint32_t replica_count = (store->node_count < store->replica_factor) ? store->node_count : store->replica_factor;

    if (replica_count > *count)
    {
        return -1;
    }

    for (uint32_t i = 0; i < replica_count && i < store->node_count; i++)
    {
        replicas[i].node_id = store->nodes[i].node_id;
        replicas[i].distance = (key_hash + i) % store->hash_ring_size;
    }

    *count = replica_count;
    return 0;
}

int kv_replicate_key(uint32_t store_id, uint32_t from_node, uint32_t to_node, const char *key)
{
    if (store_id == 0 || store_id > g_store_count || !key)
    {
        return -1;
    }

    return 0;
}

int kv_sync_node(uint32_id store_id, uint32_t node_id)
{
    if (store_id == 0 || store_id > g_store_count)
    {
        return -1;
    }

    return 0;
}

int kv_scan(uint32_t store_id, const char *key_prefix, KeyValueEntry *entries, uint32_t *count)
{
    if (store_id == 0 || store_id > g_store_count || !entries || !count)
    {
        return -1;
    }

    DistributedKVStore *store = &g_stores[store_id - 1];
    uint32_t result_count = 0;
    uint32_t prefix_len = key_prefix ? strlen(key_prefix) : 0;

    for (uint32_t i = 0; i < store->entry_count && result_count < *count; i++)
    {
        if (!store->entries[i].is_tombstone)
        {
            if (!key_prefix || strncmp(store->entries[i].key, key_prefix, prefix_len) == 0)
            {
                entries[result_count++] = store->entries[i];
            }
        }
    }

    *count = result_count;
    return 0;
}

int kv_count_keys(uint32_t store_id, uint32_t *count)
{
    if (store_id == 0 || store_id > g_store_count || !count)
    {
        return -1;
    }

    DistributedKVStore *store = &g_stores[store_id - 1];
    uint32_t key_count = 0;

    for (uint32_t i = 0; i < store->entry_count; i++)
    {
        if (!store->entries[i].is_tombstone)
        {
            key_count++;
        }
    }

    *count = key_count;
    return 0;
}

int kv_enable_compression(uint32_t store_id, int enable)
{
    if (store_id == 0 || store_id > g_store_count)
    {
        return -1;
    }

    return 0;
}

int kv_compact_store(uint32_t store_id)
{
    if (store_id == 0 || store_id > g_store_count)
    {
        return -1;
    }

    DistributedKVStore *store = &g_stores[store_id - 1];
    uint32_t write_idx = 0;

    for (uint32_t i = 0; i < store->entry_count; i++)
    {
        if (!store->entries[i].is_tombstone)
        {
            if (write_idx != i)
            {
                store->entries[write_idx] = store->entries[i];
            }
            write_idx++;
        }
    }

    store->entry_count = write_idx;
    return 0;
}

int kv_rebuild_hash_ring(uint32_id store_id)
{
    if (store_id == 0 || store_id > g_store_count)
    {
        return -1;
    }

    DistributedKVStore *store = &g_stores[store_id - 1];

    for (uint32_t i = 0; i < store->hash_ring_size; i++)
    {
        if (store->node_count > 0)
        {
            store->hash_ring[i] = store->nodes[i % store->node_count].node_id;
        }
    }

    return 0;
}

uint64_t kv_get_total_reads(uint32_t store_id)
{
    if (store_id == 0 || store_id > g_store_count)
    {
        return 0;
    }

    return g_stores[store_id - 1].total_reads;
}

uint64_t kv_get_total_writes(uint32_t store_id)
{
    if (store_id == 0 || store_id > g_store_count)
    {
        return 0;
    }

    return g_stores[store_id - 1].total_writes;
}

uint64_t kv_get_total_deletes(uint32_t store_id)
{
    if (store_id == 0 || store_id > g_store_count)
    {
        return 0;
    }

    return g_stores[store_id - 1].total_deletes;
}

uint64_t kv_get_total_bytes(uint32_t store_id)
{
    if (store_id == 0 || store_id > g_store_count)
    {
        return 0;
    }

    return g_stores[store_id - 1].bytes_total;
}

DistributedKVStore *kv_get_store_info(uint32_t store_id)
{
    if (store_id == 0 || store_id > g_store_count)
    {
        return NULL;
    }

    return &g_stores[store_id - 1];
}

int kv_export_store(uint32_t store_id, const char *format)
{
    if (store_id == 0 || store_id > g_store_count || !format)
    {
        return -1;
    }

    return 0;
}
