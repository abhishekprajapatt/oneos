#ifndef CLUSTER_MANAGER_H
#define CLUSTER_MANAGER_H

#include <stdint.h>
#include <stddef.h>
#include <time.h>

#define MAX_CLUSTER_NODES 256
#define MAX_NODE_HOSTNAME 64
#define MAX_CLUSTER_GROUPS 32
#define MAX_HEARTBEAT_TIMEOUT 10
#define MAX_REPLICATION_FACTOR 5

typedef enum
{
    NODE_HEALTHY = 0,
    NODE_DEGRADED = 1,
    NODE_UNHEALTHY = 2,
    NODE_OFFLINE = 3,
    NODE_REJOINING = 4
} NodeHealth;

typedef enum
{
    CONSENSUS_RAFT = 0,
    CONSENSUS_PAXOS = 1,
    CONSENSUS_BYZANTINE = 2
} ConsensusAlgorithm;

typedef enum
{
    REPLICATION_SYNC = 0,
    REPLICATION_ASYNC = 1,
    REPLICATION_SEMISYNC = 2
} ReplicationMode;

typedef struct
{
    uint32_t node_id;
    char hostname[MAX_NODE_HOSTNAME];
    uint32_t ip_address;
    uint16_t port;
    NodeHealth health;
    uint32_t cpu_cores;
    uint64_t memory_bytes;
    uint32_t disk_mb;
    uint64_t last_heartbeat;
    uint32_t heartbeat_interval;
    uint32_t latency_ms;
    uint64_t packets_sent;
    uint64_t packets_received;
    uint64_t bytes_sent;
    uint64_t bytes_received;
    uint8_t replication_factor;
} ClusterNode;

typedef struct
{
    uint64_t term;
    uint32_t voted_for;
    uint32_t log_index;
    uint32_t commit_index;
    uint32_t last_applied;
} RaftState;

typedef struct
{
    uint32_t cluster_id;
    char cluster_name[64];
    uint32_t node_count;
    ClusterNode nodes[MAX_CLUSTER_NODES];
    uint32_t leader_node_id;
    ConsensusAlgorithm consensus_algo;
    ReplicationMode replication_mode;
    RaftState raft_state;
    uint32_t healthy_node_count;
    uint32_t group_count;
    uint64_t total_data_replicated;
    uint64_t replication_lag_ms;
} ClusterInfo;

typedef struct
{
    uint32_t group_id;
    char group_name[64];
    uint32_t member_count;
    uint32_t member_ids[64];
    uint8_t is_active;
} NodeGroup;

typedef struct
{
    uint32_t node_id;
    uint64_t timestamp;
    uint32_t status_code;
    char message[256];
} ClusterEvent;

// Cluster initialization and management
int cluster_create(const char *name, ConsensusAlgorithm algo);
int cluster_add_node(uint32_t cluster_id, const char *hostname, uint32_t ip, uint16_t port);
int cluster_remove_node(uint32_t cluster_id, uint32_t node_id);
int cluster_join_node(uint32_t cluster_id, uint32_t node_id);
int cluster_leave_node(uint32_t cluster_id, uint32_t node_id);

// Node health and monitoring
int cluster_update_node_health(uint32_t cluster_id, uint32_t node_id, NodeHealth health);
NodeHealth cluster_get_node_health(uint32_t cluster_id, uint32_t node_id);
int cluster_send_heartbeat(uint32_t cluster_id, uint32_t node_id);
int cluster_check_node_alive(uint32_t cluster_id, uint32_t node_id);

// Consensus and leadership
int cluster_elect_leader(uint32_t cluster_id);
uint32_t cluster_get_leader(uint32_t cluster_id);
int cluster_replicate_data(uint32_t cluster_id, const void *data, size_t size);
int cluster_sync_state(uint32_t cluster_id);

// Node grouping
int cluster_create_group(uint32_t cluster_id, const char *group_name);
int cluster_add_to_group(uint32_t cluster_id, uint32_t group_id, uint32_t node_id);
int cluster_remove_from_group(uint32_t cluster_id, uint32_t group_id, uint32_t node_id);
NodeGroup *cluster_get_group(uint32_t cluster_id, uint32_t group_id);

// Failure detection and recovery
int cluster_handle_node_failure(uint32_t cluster_id, uint32_t node_id);
int cluster_promote_replica(uint32_t cluster_id, uint32_t replica_node_id);
int cluster_initiate_failover(uint32_t cluster_id, uint32_t new_leader_id);

// Replication management
int cluster_set_replication_mode(uint32_t cluster_id, ReplicationMode mode);
int cluster_set_replication_factor(uint32_t cluster_id, uint32_t node_id, uint8_t factor);
uint64_t cluster_get_replication_lag(uint32_t cluster_id);

// Statistics and monitoring
ClusterInfo *cluster_get_info(uint32_t cluster_id);
uint32_t cluster_get_healthy_count(uint32_t cluster_id);
int cluster_record_event(uint32_t cluster_id, uint32_t node_id, const char *message);

#endif // CLUSTER_MANAGER_H
