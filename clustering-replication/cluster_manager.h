#ifndef CLUSTER_MANAGER_H
#define CLUSTER_MANAGER_H

#include <stdint.h>
#include <stddef.h>

#define MAX_CLUSTER_NODES 128
#define MAX_NODE_STATE_HISTORY 1024
#define MAX_CLUSTER_EVENTS 2048

typedef enum
{
    NODE_STATE_INITIALIZING,
    NODE_STATE_HEALTHY,
    NODE_STATE_DEGRADED,
    NODE_STATE_UNREACHABLE,
    NODE_STATE_DEAD,
    NODE_STATE_RECOVERING
} NodeState;

typedef enum
{
    NODE_ROLE_FOLLOWER,
    NODE_ROLE_CANDIDATE,
    NODE_ROLE_LEADER
} NodeRole;

typedef struct
{
    uint32_t node_id;
    uint32_t node_ip;
    uint16_t node_port;

    NodeState state;
    NodeRole role;

    uint64_t last_heartbeat;
    uint64_t join_time;
    uint32_t election_term;

    uint32_t message_sent_count;
    uint32_t message_received_count;
    float latency_ms;
} ClusterNode;

typedef struct
{
    uint32_t event_id;
    uint32_t node_id;
    NodeState prev_state;
    NodeState new_state;

    uint64_t event_time;
    uint8_t event_type;
} ClusterEvent;

typedef struct
{
    uint32_t cluster_id;
    char cluster_name[64];

    ClusterNode nodes[MAX_CLUSTER_NODES];
    uint32_t node_count;
    uint32_t leader_node_id;

    ClusterEvent events[MAX_CLUSTER_EVENTS];
    uint32_t event_count;

    uint32_t current_term;
    uint32_t committed_index;

    uint32_t total_nodes_joined;
    uint32_t total_nodes_failed;
    uint32_t leader_elections;
} ClusterManager;

ClusterManager *cluster_manager_init(const char *cluster_name);
int cluster_manager_destroy(ClusterManager *manager);

uint32_t cluster_add_node(ClusterManager *manager, uint32_t node_ip, uint16_t node_port);
int cluster_remove_node(ClusterManager *manager, uint32_t node_id);

int cluster_update_node_state(ClusterManager *manager, uint32_t node_id, NodeState state);
int cluster_update_node_role(ClusterManager *manager, uint32_t node_id, NodeRole role);

int cluster_node_heartbeat(ClusterManager *manager, uint32_t node_id);
int cluster_check_node_health(ClusterManager *manager, uint32_t node_id);

int cluster_detect_failures(ClusterManager *manager);
int cluster_trigger_election(ClusterManager *manager);
int cluster_elect_leader(ClusterManager *manager);

int cluster_log_event(ClusterManager *manager, uint32_t node_id, NodeState prev, NodeState new);
int cluster_process_event(ClusterManager *manager, uint32_t event_id);

int cluster_promote_node_to_leader(ClusterManager *manager, uint32_t node_id);
int cluster_demote_leader(ClusterManager *manager);

int cluster_get_node_info(ClusterManager *manager, uint32_t node_id, ClusterNode *node);
int cluster_get_all_nodes(ClusterManager *manager, ClusterNode *nodes, uint32_t max_count);

typedef struct
{
    uint32_t cluster_id;
    uint32_t total_nodes;
    uint32_t healthy_nodes;
    uint32_t degraded_nodes;
    uint32_t dead_nodes;
    uint32_t leader_node_id;
    uint32_t current_term;
    float average_latency_ms;
} ClusterMetrics;

int cluster_get_metrics(ClusterManager *manager, ClusterMetrics *metrics);

#endif
