#include "cluster_manager.h"
#include <stdlib.h>
#include <string.h>

static ClusterManager *global_cluster_manager = NULL;
static uint32_t next_node_id = 1;
static uint32_t next_event_id = 1;

ClusterManager *cluster_manager_init(const char *cluster_name)
{
    ClusterManager *manager = (ClusterManager *)malloc(sizeof(ClusterManager));
    if (!manager)
        return NULL;

    memset(manager, 0, sizeof(ClusterManager));
    manager->cluster_id = 1;

    if (cluster_name)
    {
        strncpy(manager->cluster_name, cluster_name, sizeof(manager->cluster_name) - 1);
    }

    manager->node_count = 0;
    manager->leader_node_id = 0;
    manager->event_count = 0;
    manager->current_term = 1;
    manager->committed_index = 0;
    manager->total_nodes_joined = 0;
    manager->total_nodes_failed = 0;
    manager->leader_elections = 0;

    global_cluster_manager = manager;
    return manager;
}

int cluster_manager_destroy(ClusterManager *manager)
{
    if (!manager)
        return -1;

    free(manager);
    global_cluster_manager = NULL;
    return 0;
}

uint32_t cluster_add_node(ClusterManager *manager, uint32_t node_ip, uint16_t node_port)
{
    if (!manager || manager->node_count >= MAX_CLUSTER_NODES)
        return 0;

    ClusterNode *node = &manager->nodes[manager->node_count];
    memset(node, 0, sizeof(ClusterNode));

    node->node_id = next_node_id++;
    node->node_ip = node_ip;
    node->node_port = node_port;
    node->state = NODE_STATE_INITIALIZING;
    node->role = NODE_ROLE_FOLLOWER;
    node->last_heartbeat = 0;
    node->join_time = 0;
    node->election_term = manager->current_term;
    node->message_sent_count = 0;
    node->message_received_count = 0;
    node->latency_ms = 0.0f;

    manager->node_count++;
    manager->total_nodes_joined++;

    return node->node_id;
}

int cluster_remove_node(ClusterManager *manager, uint32_t node_id)
{
    if (!manager || node_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->node_count; i++)
    {
        if (manager->nodes[i].node_id == node_id)
        {
            memmove(&manager->nodes[i], &manager->nodes[i + 1],
                    sizeof(ClusterNode) * (manager->node_count - i - 1));
            manager->node_count--;

            if (manager->leader_node_id == node_id)
            {
                manager->leader_node_id = 0;
            }

            return 0;
        }
    }

    return -1;
}

int cluster_update_node_state(ClusterManager *manager, uint32_t node_id, NodeState state)
{
    if (!manager || node_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->node_count; i++)
    {
        if (manager->nodes[i].node_id == node_id)
        {
            ClusterNode *node = &manager->nodes[i];
            NodeState prev_state = node->state;
            node->state = state;

            cluster_log_event(manager, node_id, prev_state, state);
            return 0;
        }
    }

    return -1;
}

int cluster_update_node_role(ClusterManager *manager, uint32_t node_id, NodeRole role)
{
    if (!manager || node_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->node_count; i++)
    {
        if (manager->nodes[i].node_id == node_id)
        {
            manager->nodes[i].role = role;
            return 0;
        }
    }

    return -1;
}

int cluster_node_heartbeat(ClusterManager *manager, uint32_t node_id)
{
    if (!manager || node_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->node_count; i++)
    {
        if (manager->nodes[i].node_id == node_id)
        {
            ClusterNode *node = &manager->nodes[i];
            node->last_heartbeat = 0;
            node->message_received_count++;

            if (node->state == NODE_STATE_UNREACHABLE ||
                node->state == NODE_STATE_RECOVERING)
            {
                node->state = NODE_STATE_HEALTHY;
            }

            return 0;
        }
    }

    return -1;
}

int cluster_check_node_health(ClusterManager *manager, uint32_t node_id)
{
    if (!manager || node_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->node_count; i++)
    {
        if (manager->nodes[i].node_id == node_id)
        {
            ClusterNode *node = &manager->nodes[i];

            if (node->state == NODE_STATE_HEALTHY)
            {
                return 1;
            }
            else if (node->state == NODE_STATE_DEGRADED)
            {
                return 0;
            }
            else
            {
                return -1;
            }
        }
    }

    return -1;
}

int cluster_detect_failures(ClusterManager *manager)
{
    if (!manager)
        return 0;

    uint32_t failed_count = 0;

    for (uint32_t i = 0; i < manager->node_count; i++)
    {
        ClusterNode *node = &manager->nodes[i];

        if (node->state == NODE_STATE_UNREACHABLE || node->state == NODE_STATE_DEAD)
        {
            failed_count++;
            manager->total_nodes_failed++;
        }
    }

    return failed_count;
}

int cluster_trigger_election(ClusterManager *manager)
{
    if (!manager)
        return -1;

    manager->current_term++;
    manager->leader_elections++;

    for (uint32_t i = 0; i < manager->node_count; i++)
    {
        manager->nodes[i].election_term = manager->current_term;
        if (manager->nodes[i].role != NODE_ROLE_LEADER)
        {
            manager->nodes[i].role = NODE_ROLE_CANDIDATE;
        }
    }

    return 0;
}

int cluster_elect_leader(ClusterManager *manager)
{
    if (!manager || manager->node_count == 0)
        return -1;

    for (uint32_t i = 0; i < manager->node_count; i++)
    {
        if (manager->nodes[i].state == NODE_STATE_HEALTHY)
        {
            manager->leader_node_id = manager->nodes[i].node_id;
            manager->nodes[i].role = NODE_ROLE_LEADER;
            return 0;
        }
    }

    return -1;
}

int cluster_log_event(ClusterManager *manager, uint32_t node_id, NodeState prev, NodeState new)
{
    if (!manager || manager->event_count >= MAX_CLUSTER_EVENTS)
        return -1;

    ClusterEvent *event = &manager->events[manager->event_count];
    memset(event, 0, sizeof(ClusterEvent));

    event->event_id = next_event_id++;
    event->node_id = node_id;
    event->prev_state = prev;
    event->new_state = new;
    event->event_time = 0;
    event->event_type = 1;

    manager->event_count++;
    return event->event_id;
}

int cluster_process_event(ClusterManager *manager, uint32_t event_id)
{
    if (!manager || event_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->event_count; i++)
    {
        if (manager->events[i].event_id == event_id)
        {
            return 0;
        }
    }

    return -1;
}

int cluster_promote_node_to_leader(ClusterManager *manager, uint32_t node_id)
{
    if (!manager || node_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->node_count; i++)
    {
        if (manager->nodes[i].node_id == node_id)
        {
            if (manager->leader_node_id > 0)
            {
                for (uint32_t j = 0; j < manager->node_count; j++)
                {
                    if (manager->nodes[j].node_id == manager->leader_node_id)
                    {
                        manager->nodes[j].role = NODE_ROLE_FOLLOWER;
                        break;
                    }
                }
            }

            manager->nodes[i].role = NODE_ROLE_LEADER;
            manager->leader_node_id = node_id;
            manager->leader_elections++;
            return 0;
        }
    }

    return -1;
}

int cluster_demote_leader(ClusterManager *manager)
{
    if (!manager || manager->leader_node_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->node_count; i++)
    {
        if (manager->nodes[i].node_id == manager->leader_node_id)
        {
            manager->nodes[i].role = NODE_ROLE_FOLLOWER;
            manager->leader_node_id = 0;
            return 0;
        }
    }

    return -1;
}

int cluster_get_node_info(ClusterManager *manager, uint32_t node_id, ClusterNode *node)
{
    if (!manager || node_id == 0 || !node)
        return -1;

    for (uint32_t i = 0; i < manager->node_count; i++)
    {
        if (manager->nodes[i].node_id == node_id)
        {
            memcpy(node, &manager->nodes[i], sizeof(ClusterNode));
            return 0;
        }
    }

    return -1;
}

int cluster_get_all_nodes(ClusterManager *manager, ClusterNode *nodes, uint32_t max_count)
{
    if (!manager || !nodes || max_count == 0)
        return -1;

    uint32_t to_copy = manager->node_count < max_count ? manager->node_count : max_count;
    memcpy(nodes, manager->nodes, sizeof(ClusterNode) * to_copy);

    return to_copy;
}

int cluster_get_metrics(ClusterManager *manager, ClusterMetrics *metrics)
{
    if (!manager || !metrics)
        return -1;

    uint32_t healthy = 0, degraded = 0, dead = 0;
    float total_latency = 0.0f;

    for (uint32_t i = 0; i < manager->node_count; i++)
    {
        if (manager->nodes[i].state == NODE_STATE_HEALTHY)
            healthy++;
        else if (manager->nodes[i].state == NODE_STATE_DEGRADED)
            degraded++;
        else if (manager->nodes[i].state == NODE_STATE_DEAD)
            dead++;

        total_latency += manager->nodes[i].latency_ms;
    }

    metrics->cluster_id = manager->cluster_id;
    metrics->total_nodes = manager->node_count;
    metrics->healthy_nodes = healthy;
    metrics->degraded_nodes = degraded;
    metrics->dead_nodes = dead;
    metrics->leader_node_id = manager->leader_node_id;
    metrics->current_term = manager->current_term;

    if (manager->node_count > 0)
    {
        metrics->average_latency_ms = total_latency / manager->node_count;
    }
    else
    {
        metrics->average_latency_ms = 0.0f;
    }

    return 0;
}
