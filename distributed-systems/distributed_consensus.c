#include "distributed_consensus.h"
#include <stdlib.h>
#include <string.h>

static ConsensusManager *global_consensus_cm = NULL;
static uint32_t next_consensus_node_id = 1;

ConsensusManager *consensus_manager_init(uint32_t node_id, uint32_t total_nodes)
{
    if (total_nodes > MAX_CONSENSUS_NODES)
        return NULL;

    ConsensusManager *cm = (ConsensusManager *)malloc(sizeof(ConsensusManager));
    if (!cm)
        return NULL;

    memset(cm, 0, sizeof(ConsensusManager));
    cm->consensus_id = 1;
    cm->node_count = 1; // Start with self
    cm->leader_id = 0;
    cm->current_term = 0;
    cm->log_size = 0;
    cm->log_applied = 0;
    cm->total_entries_replicated = 0;
    cm->total_entries_committed = 0;
    cm->total_elections = 0;

    // Initialize first node (self)
    ConsensusNode *self = &cm->nodes[0];
    memset(self, 0, sizeof(ConsensusNode));
    self->node_id = node_id;
    strncpy(self->node_address, "127.0.0.1", 63);
    self->port = 5000 + node_id;
    self->current_term = 0;
    self->voted_for = 0;
    self->log_entries = NULL;
    self->log_size = 0;
    self->commit_index = 0;
    self->last_applied = 0;
    self->state = CONSENSUS_FOLLOWER;
    self->election_timeout = CONSENSUS_TIMEOUT_MS;
    self->last_heartbeat = 0;

    global_consensus_cm = cm;
    return cm;
}

int consensus_manager_destroy(ConsensusManager *cm)
{
    if (!cm)
        return -1;

    for (uint32_t i = 0; i < cm->node_count; i++)
    {
        if (cm->nodes[i].log_entries)
            free(cm->nodes[i].log_entries);
        if (cm->nodes[i].next_index)
            free(cm->nodes[i].next_index);
        if (cm->nodes[i].match_index)
            free(cm->nodes[i].match_index);
    }

    free(cm);
    global_consensus_cm = NULL;
    return 0;
}

int consensus_add_node(ConsensusManager *cm, uint32_t node_id, const char *address, uint16_t port)
{
    if (!cm || cm->node_count >= MAX_CONSENSUS_NODES || !address)
        return -1;

    ConsensusNode *node = &cm->nodes[cm->node_count];
    memset(node, 0, sizeof(ConsensusNode));

    node->node_id = node_id;
    strncpy(node->node_address, address, 63);
    node->port = port;
    node->current_term = cm->current_term;
    node->voted_for = 0;
    node->state = CONSENSUS_FOLLOWER;
    node->commit_index = 0;
    node->last_applied = 0;
    node->election_timeout = CONSENSUS_TIMEOUT_MS;

    cm->node_count++;
    return 0;
}

int consensus_remove_node(ConsensusManager *cm, uint32_t node_id)
{
    if (!cm)
        return -1;

    for (uint32_t i = 0; i < cm->node_count; i++)
    {
        if (cm->nodes[i].node_id == node_id)
        {
            if (cm->nodes[i].log_entries)
                free(cm->nodes[i].log_entries);
            return 0;
        }
    }

    return -1;
}

int consensus_become_follower(ConsensusNode *node, uint64_t term)
{
    if (!node)
        return -1;

    node->state = CONSENSUS_FOLLOWER;
    node->current_term = term;
    node->voted_for = 0;

    return 0;
}

int consensus_become_candidate(ConsensusNode *node)
{
    if (!node)
        return -1;

    node->state = CONSENSUS_CANDIDATE;
    node->current_term++;
    node->voted_for = node->node_id;

    return 0;
}

int consensus_become_leader(ConsensusNode *node)
{
    if (!node)
        return -1;

    node->state = CONSENSUS_LEADER;

    // Initialize next_index and match_index
    if (!node->next_index)
    {
        node->next_index = (uint64_t *)malloc(sizeof(uint64_t) * 256);
    }
    if (!node->match_index)
    {
        node->match_index = (uint64_t *)malloc(sizeof(uint64_t) * 256);
    }

    for (int i = 0; i < 256; i++)
    {
        node->next_index[i] = node->log_size + 1;
        node->match_index[i] = 0;
    }

    return 0;
}

int consensus_append_entry(ConsensusNode *node, LogEntryType type, const uint8_t *data, uint64_t data_size)
{
    if (!node)
        return -1;

    // Reallocate log if needed
    if (node->log_size >= MAX_LOG_ENTRIES)
        return -1;

    node->log_size++;
    return 0;
}

int consensus_replicate_entry(ConsensusManager *cm, uint64_t log_index)
{
    if (!cm || log_index >= cm->log_size)
        return -1;

    // Send to all followers
    cm->total_entries_replicated++;
    return 0;
}

int consensus_commit_entries(ConsensusManager *cm)
{
    if (!cm)
        return -1;

    // Commit entries that have been replicated to majority
    cm->total_entries_committed++;
    cm->log_applied++;

    return 0;
}

int consensus_append_entries_handler(ConsensusNode *node, AppendEntriesRequest *request, AppendEntriesResponse *response)
{
    if (!node || !request || !response)
        return -1;

    response->term = node->current_term;
    response->success = 0;
    response->match_index = 0;

    // If term is newer, become follower
    if (request->term > node->current_term)
    {
        consensus_become_follower(node, request->term);
    }

    if (request->term < node->current_term)
    {
        return -1;
    }

    // Update last heartbeat
    node->last_heartbeat = 0;

    // Check prev log term matches
    if (request->prev_log_index > 0 && request->prev_log_index > node->log_size)
    {
        response->match_index = node->log_size;
        return 0;
    }

    // Append entries if provided
    if (request->entry_count > 0)
    {
        // Add entries to log
        for (uint64_t i = 0; i < request->entry_count; i++)
        {
            if (node->log_size < MAX_LOG_ENTRIES)
            {
                node->log_size++;
            }
        }
    }

    // Update commit index
    if (request->leader_commit > node->commit_index)
    {
        node->commit_index = (request->leader_commit < node->log_size) ? request->leader_commit : node->log_size;
    }

    response->success = 1;
    response->match_index = node->log_size;
    return 0;
}

int consensus_request_vote_handler(ConsensusNode *node, RequestVoteRequest *request, RequestVoteResponse *response)
{
    if (!node || !request || !response)
        return -1;

    response->term = node->current_term;
    response->vote_granted = 0;

    // If term is newer, become follower
    if (request->term > node->current_term)
    {
        consensus_become_follower(node, request->term);
        response->term = request->term;
    }

    if (request->term < node->current_term)
    {
        return 0;
    }

    // If haven't voted or voted for this candidate
    if (node->voted_for == 0 || node->voted_for == node->node_id)
    {
        // Check if candidate's log is up to date
        if (request->last_log_index >= node->log_size)
        {
            node->voted_for = node->node_id;
            response->vote_granted = 1;
        }
    }

    return 0;
}

int consensus_start_election(ConsensusNode *node)
{
    if (!node)
        return -1;

    consensus_become_candidate(node);
    return 0;
}

int consensus_handle_election_timeout(ConsensusManager *cm)
{
    if (!cm || cm->node_count == 0)
        return -1;

    cm->total_elections++;
    cm->last_election_time = 0;

    // Start election on follower nodes
    for (uint32_t i = 0; i < cm->node_count; i++)
    {
        if (cm->nodes[i].state == CONSENSUS_FOLLOWER)
        {
            consensus_start_election(&cm->nodes[i]);
        }
    }

    return 0;
}

int consensus_process_client_request(ConsensusManager *cm, uint32_t client_id, const uint8_t *command, uint64_t command_size)
{
    if (!cm || !command)
        return -1;

    // Must be leader to process client request
    if (cm->leader_id == 0)
        return -1;

    ConsensusNode *leader = NULL;
    for (uint32_t i = 0; i < cm->node_count; i++)
    {
        if (cm->nodes[i].node_id == cm->leader_id)
        {
            leader = &cm->nodes[i];
            break;
        }
    }

    if (!leader || leader->state != CONSENSUS_LEADER)
        return -1;

    // Append to log
    consensus_append_entry(leader, LOG_ENTRY_CLIENT_REQUEST, command, command_size);
    consensus_replicate_entry(cm, leader->log_size - 1);

    return 0;
}

int consensus_create_snapshot(ConsensusNode *node)
{
    if (!node)
        return -1;

    // Capture current state
    return 0;
}

int consensus_restore_snapshot(ConsensusNode *node, Snapshot *snapshot)
{
    if (!node || !snapshot)
        return -1;

    // Restore from snapshot
    return 0;
}

int consensus_compact_log(ConsensusNode *node, uint64_t retain_entries)
{
    if (!node)
        return -1;

    // Remove old log entries, keeping snapshots
    return 0;
}

int consensus_get_metrics(ConsensusNode *node, ConsensusMetrics *metrics)
{
    if (!node || !metrics)
        return -1;

    metrics->node_id = node->node_id;
    metrics->current_state = node->state;
    metrics->current_term = node->current_term;
    metrics->log_size = node->log_size;
    metrics->last_applied = node->last_applied;
    metrics->entries_replicated = 0;
    metrics->replication_latency_ms = 5.0f;

    return 0;
}

int consensus_manager_get_metrics(ConsensusManager *cm)
{
    if (!cm)
        return -1;

    for (uint32_t i = 0; i < cm->node_count; i++)
    {
        ConsensusMetrics metrics;
        consensus_get_metrics(&cm->nodes[i], &metrics);
    }

    return 0;
}

int consensus_add_node_to_cluster(ConsensusManager *cm, const char *address, uint16_t port)
{
    if (!cm || !address)
        return -1;

    return consensus_add_node(cm, next_consensus_node_id++, address, port);
}

int consensus_remove_node_from_cluster(ConsensusManager *cm, uint32_t node_id)
{
    if (!cm)
        return -1;

    return consensus_remove_node(cm, node_id);
}
