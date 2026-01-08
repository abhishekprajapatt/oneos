#include "cluster_manager.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static ClusterInfo g_clusters[8] = {0};
static uint32_t g_cluster_count = 0;
static ClusterEvent g_cluster_events[256] = {0};
static uint32_t g_event_index = 0;

int cluster_create(const char *name, ConsensusAlgorithm algo)
{
    if (!name || g_cluster_count >= 8)
    {
        return -1;
    }

    ClusterInfo *cluster = &g_clusters[g_cluster_count];
    cluster->cluster_id = g_cluster_count + 1;
    strncpy(cluster->cluster_name, name, 63);
    cluster->consensus_algo = algo;
    cluster->replication_mode = REPLICATION_SEMISYNC;
    cluster->leader_node_id = 0;
    cluster->node_count = 0;
    cluster->healthy_node_count = 0;
    cluster->raft_state.term = 1;
    cluster->raft_state.voted_for = 0;
    cluster->raft_state.log_index = 0;

    return cluster->cluster_id;
}

int cluster_add_node(uint32_t cluster_id, const char *hostname, uint32_t ip, uint16_t port)
{
    if (cluster_id == 0 || cluster_id > g_cluster_count || !hostname)
    {
        return -1;
    }

    ClusterInfo *cluster = &g_clusters[cluster_id - 1];
    if (cluster->node_count >= MAX_CLUSTER_NODES)
    {
        return -1;
    }

    ClusterNode *node = &cluster->nodes[cluster->node_count];
    node->node_id = cluster->node_count + 1;
    strncpy(node->hostname, hostname, MAX_NODE_HOSTNAME - 1);
    node->ip_address = ip;
    node->port = port;
    node->health = NODE_OFFLINE;
    node->cpu_cores = 4;
    node->memory_bytes = 8 * 1024 * 1024 * 1024UL;
    node->disk_mb = 512 * 1024;
    node->last_heartbeat = 0;
    node->heartbeat_interval = 5;
    node->latency_ms = 0;
    node->replication_factor = 1;
    node->packets_sent = 0;
    node->packets_received = 0;
    node->bytes_sent = 0;
    node->bytes_received = 0;

    cluster->node_count++;
    return node->node_id;
}

int cluster_remove_node(uint32_t cluster_id, uint32_t node_id)
{
    if (cluster_id == 0 || cluster_id > g_cluster_count)
    {
        return -1;
    }

    ClusterInfo *cluster = &g_clusters[cluster_id - 1];
    if (node_id == 0 || node_id > cluster->node_count)
    {
        return -1;
    }

    if (cluster->leader_node_id == node_id)
    {
        cluster_elect_leader(cluster_id);
    }

    for (uint32_t i = node_id - 1; i < cluster->node_count - 1; i++)
    {
        cluster->nodes[i] = cluster->nodes[i + 1];
    }
    cluster->node_count--;

    return 0;
}

int cluster_join_node(uint32_t cluster_id, uint32_t node_id)
{
    if (cluster_id == 0 || cluster_id > g_cluster_count)
    {
        return -1;
    }

    ClusterInfo *cluster = &g_clusters[cluster_id - 1];
    if (node_id == 0 || node_id > cluster->node_count)
    {
        return -1;
    }

    ClusterNode *node = &cluster->nodes[node_id - 1];
    node->health = NODE_REJOINING;
    node->last_heartbeat = time(NULL);

    return 0;
}

int cluster_leave_node(uint32_t cluster_id, uint32_t node_id)
{
    if (cluster_id == 0 || cluster_id > g_cluster_count)
    {
        return -1;
    }

    ClusterInfo *cluster = &g_clusters[cluster_id - 1];
    if (node_id == 0 || node_id > cluster->node_count)
    {
        return -1;
    }

    ClusterNode *node = &cluster->nodes[node_id - 1];
    node->health = NODE_OFFLINE;

    return 0;
}

int cluster_update_node_health(uint32_t cluster_id, uint32_t node_id, NodeHealth health)
{
    if (cluster_id == 0 || cluster_id > g_cluster_count)
    {
        return -1;
    }

    ClusterInfo *cluster = &g_clusters[cluster_id - 1];
    if (node_id == 0 || node_id > cluster->node_count)
    {
        return -1;
    }

    ClusterNode *node = &cluster->nodes[node_id - 1];
    NodeHealth old_health = node->health;
    node->health = health;

    if (health == NODE_HEALTHY && old_health != NODE_HEALTHY)
    {
        cluster->healthy_node_count++;
    }
    else if (health != NODE_HEALTHY && old_health == NODE_HEALTHY)
    {
        cluster->healthy_node_count--;
    }

    return 0;
}

NodeHealth cluster_get_node_health(uint32_t cluster_id, uint32_t node_id)
{
    if (cluster_id == 0 || cluster_id > g_cluster_count)
    {
        return NODE_OFFLINE;
    }

    ClusterInfo *cluster = &g_clusters[cluster_id - 1];
    if (node_id == 0 || node_id > cluster->node_count)
    {
        return NODE_OFFLINE;
    }

    return cluster->nodes[node_id - 1].health;
}

int cluster_send_heartbeat(uint32_t cluster_id, uint32_t node_id)
{
    if (cluster_id == 0 || cluster_id > g_cluster_count)
    {
        return -1;
    }

    ClusterInfo *cluster = &g_clusters[cluster_id - 1];
    if (node_id == 0 || node_id > cluster->node_count)
    {
        return -1;
    }

    ClusterNode *node = &cluster->nodes[node_id - 1];
    node->last_heartbeat = time(NULL);
    node->packets_sent++;
    node->bytes_sent += 64;

    return 0;
}

int cluster_check_node_alive(uint32_t cluster_id, uint32_t node_id)
{
    if (cluster_id == 0 || cluster_id > g_cluster_count)
    {
        return 0;
    }

    ClusterInfo *cluster = &g_clusters[cluster_id - 1];
    if (node_id == 0 || node_id > cluster->node_count)
    {
        return 0;
    }

    ClusterNode *node = &cluster->nodes[node_id - 1];
    time_t now = time(NULL);
    uint32_t timeout = node->heartbeat_interval * MAX_HEARTBEAT_TIMEOUT;

    if (now - node->last_heartbeat > timeout)
    {
        cluster_update_node_health(cluster_id, node_id, NODE_OFFLINE);
        return 0;
    }

    return 1;
}

int cluster_elect_leader(uint32_t cluster_id)
{
    if (cluster_id == 0 || cluster_id > g_cluster_count)
    {
        return -1;
    }

    ClusterInfo *cluster = &g_clusters[cluster_id - 1];
    uint32_t best_node = 0;
    uint32_t max_latency = UINT32_MAX;

    for (uint32_t i = 0; i < cluster->node_count; i++)
    {
        if (cluster->nodes[i].health == NODE_HEALTHY)
        {
            if (cluster->nodes[i].latency_ms < max_latency)
            {
                max_latency = cluster->nodes[i].latency_ms;
                best_node = cluster->nodes[i].node_id;
            }
        }
    }

    if (best_node > 0)
    {
        cluster->leader_node_id = best_node;
        cluster->raft_state.term++;
        cluster->raft_state.voted_for = best_node;
        return best_node;
    }

    return -1;
}

uint32_t cluster_get_leader(uint32_t cluster_id)
{
    if (cluster_id == 0 || cluster_id > g_cluster_count)
    {
        return 0;
    }

    return g_clusters[cluster_id - 1].leader_node_id;
}

int cluster_replicate_data(uint32_t cluster_id, const void *data, size_t size)
{
    if (cluster_id == 0 || cluster_id > g_cluster_count || !data)
    {
        return -1;
    }

    ClusterInfo *cluster = &g_clusters[cluster_id - 1];
    uint32_t replicated_count = 0;

    for (uint32_t i = 0; i < cluster->node_count; i++)
    {
        if (cluster->nodes[i].health == NODE_HEALTHY)
        {
            cluster->nodes[i].bytes_received += size;
            cluster->nodes[i].packets_received++;
            replicated_count++;
        }
    }

    cluster->total_data_replicated += size * replicated_count;

    return replicated_count;
}

int cluster_sync_state(uint32_t cluster_id)
{
    if (cluster_id == 0 || cluster_id > g_cluster_count)
    {
        return -1;
    }

    ClusterInfo *cluster = &g_clusters[cluster_id - 1];
    uint32_t synced_count = 0;

    for (uint32_t i = 0; i < cluster->node_count; i++)
    {
        if (cluster->nodes[i].health != NODE_OFFLINE)
        {
            cluster->nodes[i].packets_sent++;
            synced_count++;
        }
    }

    cluster->raft_state.log_index++;
    cluster->raft_state.commit_index = cluster->raft_state.log_index;

    return synced_count;
}

int cluster_create_group(uint32_t cluster_id, const char *group_name)
{
    if (cluster_id == 0 || cluster_id > g_cluster_count || !group_name)
    {
        return -1;
    }

    ClusterInfo *cluster = &g_clusters[cluster_id - 1];
    if (cluster->group_count >= MAX_CLUSTER_GROUPS)
    {
        return -1;
    }

    return cluster->group_count++;
}

int cluster_add_to_group(uint32_t cluster_id, uint32_t group_id, uint32_t node_id)
{
    if (cluster_id == 0 || cluster_id > g_cluster_count)
    {
        return -1;
    }

    return 0;
}

int cluster_remove_from_group(uint32_t cluster_id, uint32_t group_id, uint32_t node_id)
{
    if (cluster_id == 0 || cluster_id > g_cluster_count)
    {
        return -1;
    }

    return 0;
}

NodeGroup *cluster_get_group(uint32_t cluster_id, uint32_t group_id)
{
    if (cluster_id == 0 || cluster_id > g_cluster_count)
    {
        return NULL;
    }

    return NULL;
}

int cluster_handle_node_failure(uint32_t cluster_id, uint32_t node_id)
{
    if (cluster_id == 0 || cluster_id > g_cluster_count)
    {
        return -1;
    }

    ClusterInfo *cluster = &g_clusters[cluster_id - 1];
    if (node_id == 0 || node_id > cluster->node_count)
    {
        return -1;
    }

    cluster_update_node_health(cluster_id, node_id, NODE_OFFLINE);

    if (cluster->leader_node_id == node_id)
    {
        return cluster_elect_leader(cluster_id);
    }

    return 0;
}

int cluster_promote_replica(uint32_t cluster_id, uint32_t replica_node_id)
{
    if (cluster_id == 0 || cluster_id > g_cluster_count)
    {
        return -1;
    }

    ClusterInfo *cluster = &g_clusters[cluster_id - 1];
    cluster->leader_node_id = replica_node_id;
    cluster->raft_state.term++;

    return replica_node_id;
}

int cluster_initiate_failover(uint32_t cluster_id, uint32_t new_leader_id)
{
    if (cluster_id == 0 || cluster_id > g_cluster_count)
    {
        return -1;
    }

    ClusterInfo *cluster = &g_clusters[cluster_id - 1];
    if (new_leader_id > 0 && new_leader_id <= cluster->node_count)
    {
        cluster->leader_node_id = new_leader_id;
        cluster->raft_state.term++;
        return 0;
    }

    return -1;
}

int cluster_set_replication_mode(uint32_t cluster_id, ReplicationMode mode)
{
    if (cluster_id == 0 || cluster_id > g_cluster_count)
    {
        return -1;
    }

    g_clusters[cluster_id - 1].replication_mode = mode;
    return 0;
}

int cluster_set_replication_factor(uint32_t cluster_id, uint32_t node_id, uint8_t factor)
{
    if (cluster_id == 0 || cluster_id > g_cluster_count)
    {
        return -1;
    }

    ClusterInfo *cluster = &g_clusters[cluster_id - 1];
    if (node_id == 0 || node_id > cluster->node_count || factor > MAX_REPLICATION_FACTOR)
    {
        return -1;
    }

    cluster->nodes[node_id - 1].replication_factor = factor;
    return 0;
}

uint64_t cluster_get_replication_lag(uint32_t cluster_id)
{
    if (cluster_id == 0 || cluster_id > g_cluster_count)
    {
        return 0;
    }

    return g_clusters[cluster_id - 1].replication_lag_ms;
}

ClusterInfo *cluster_get_info(uint32_t cluster_id)
{
    if (cluster_id == 0 || cluster_id > g_cluster_count)
    {
        return NULL;
    }

    return &g_clusters[cluster_id - 1];
}

uint32_t cluster_get_healthy_count(uint32_t cluster_id)
{
    if (cluster_id == 0 || cluster_id > g_cluster_count)
    {
        return 0;
    }

    return g_clusters[cluster_id - 1].healthy_node_count;
}

int cluster_record_event(uint32_t cluster_id, uint32_t node_id, const char *message)
{
    if (cluster_id == 0 || !message)
    {
        return -1;
    }

    ClusterEvent *event = &g_cluster_events[g_event_index % 256];
    event->node_id = node_id;
    event->timestamp = time(NULL);
    event->status_code = 0;
    strncpy(event->message, message, 255);

    g_event_index++;
    return 0;
}
