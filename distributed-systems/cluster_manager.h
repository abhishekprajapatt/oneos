#ifndef CLUSTER_MANAGER_H
#define CLUSTER_MANAGER_H

#include <stdint.h>
#include <stddef.h>

#define MAX_CLUSTER_NODES 256
#define MAX_NODE_NAME_LEN 64
#define MAX_CLUSTER_NAME_LEN 32
#define MAX_NODE_METADATA 512

typedef enum
{
    NODE_STATE_UNKNOWN,
    NODE_STATE_HEALTHY,
    NODE_STATE_UNHEALTHY,
    NODE_STATE_DEGRADED,
    NODE_STATE_OFFLINE,
    NODE_STATE_JOINING,
    NODE_STATE_LEAVING
} NodeHealthState;

typedef enum
{
    CLUSTER_ROLE_MASTER,
    CLUSTER_ROLE_WORKER,
    CLUSTER_ROLE_REPLICA,
    CLUSTER_ROLE_SENTINEL
} ClusterRole;

typedef struct
{
    uint32_t node_id;
    char node_name[MAX_NODE_NAME_LEN];
    char node_address[64];
    uint16_t port;

    ClusterRole role;
    NodeHealthState health_state;

    uint64_t cpu_count;
    uint64_t memory_total;
    uint64_t memory_available;
    uint64_t disk_total;
    uint64_t disk_available;

    uint32_t heartbeat_interval;
    uint32_t last_heartbeat_time;
    uint32_t join_time;
    uint32_t uptime_seconds;

    uint64_t tasks_running;
    uint64_t tasks_completed;
    uint64_t tasks_failed;

    float cpu_usage_percent;
    float memory_usage_percent;

    uint8_t metadata[MAX_NODE_METADATA];
    uint32_t metadata_size;
} ClusterNode;

typedef struct
{
    uint32_t cluster_id;
    char cluster_name[MAX_CLUSTER_NAME_LEN];

    uint32_t node_count;
    ClusterNode nodes[MAX_CLUSTER_NODES];

    uint32_t master_node_id;
    uint32_t replica_node_id;

    uint32_t total_nodes_ever;
    uint32_t total_nodes_failed;

    uint64_t total_cpu_cores;
    uint64_t total_memory;
    uint64_t total_disk;

    uint64_t total_tasks_completed;
    uint64_t total_tasks_failed;

    uint32_t cluster_created_time;
    uint32_t last_heartbeat_check;
} Cluster;

typedef struct
{
    uint32_t cluster_manager_id;

    uint32_t cluster_count;
    Cluster clusters[16]; // Support multiple clusters

    uint32_t node_discovery_enabled;
    uint32_t auto_failover_enabled;
    uint32_t health_check_interval;

    uint64_t total_nodes_managed;
    uint64_t total_alive_nodes;
} ClusterManager;

// Cluster Manager initialization
ClusterManager *cluster_manager_init(void);
int cluster_manager_destroy(ClusterManager *cm);

// Cluster operations
Cluster *cluster_create(ClusterManager *cm, const char *cluster_name);
int cluster_destroy(ClusterManager *cm, uint32_t cluster_id);
int cluster_join(Cluster *cluster, const char *node_name, const char *node_address, uint16_t port);
int cluster_leave(Cluster *cluster, uint32_t node_id);

// Node management
ClusterNode *cluster_get_node(Cluster *cluster, uint32_t node_id);
int cluster_update_node_status(ClusterNode *node, NodeHealthState state);
int cluster_update_node_metrics(ClusterNode *node, float cpu_usage, float mem_usage);

// Health checking
int cluster_perform_health_check(Cluster *cluster);
int cluster_node_heartbeat(ClusterNode *node);
int cluster_handle_node_failure(Cluster *cluster, uint32_t node_id);

// Node discovery
int cluster_discover_nodes(ClusterManager *cm);
int cluster_register_node(Cluster *cluster, const char *node_address);
int cluster_deregister_node(Cluster *cluster, uint32_t node_id);

// Leader election
int cluster_elect_master(Cluster *cluster);
int cluster_elect_replica(Cluster *cluster);
ClusterNode *cluster_get_master(Cluster *cluster);
ClusterNode *cluster_get_replica(Cluster *cluster);

// Monitoring
typedef struct
{
    uint32_t cluster_id;
    uint32_t active_nodes;
    uint32_t failed_nodes;
    float cluster_cpu_usage;
    float cluster_memory_usage;
    uint64_t total_tasks_running;
    float cluster_availability_percent;
} ClusterMetrics;

int cluster_get_metrics(Cluster *cluster, ClusterMetrics *metrics);
int cluster_manager_get_metrics(ClusterManager *cm);

// Cluster rebalancing
int cluster_rebalance_load(Cluster *cluster);
int cluster_redistribute_tasks(Cluster *cluster, uint32_t source_node_id);

// Node affinity and constraints
typedef struct
{
    uint32_t node_id;
    uint32_t preferred_nodes[16];
    uint8_t constraints[64];
} NodeAffinity;

int cluster_set_node_affinity(ClusterNode *node, NodeAffinity *affinity);
int cluster_schedule_with_affinity(Cluster *cluster, NodeAffinity *affinity);

#endif // CLUSTER_MANAGER_H
