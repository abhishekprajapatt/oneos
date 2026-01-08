#include "consensus_engine.h"
#include <stdlib.h>
#include <string.h>

static ConsensusEngine *global_consensus_engine = NULL;
static uint32_t next_quorum_op_id = 1;

ConsensusEngine *consensus_engine_init(uint32_t node_id)
{
    ConsensusEngine *engine = (ConsensusEngine *)malloc(sizeof(ConsensusEngine));
    if (!engine)
        return NULL;

    memset(engine, 0, sizeof(ConsensusEngine));
    engine->engine_id = 1;
    engine->node_id = node_id;
    engine->state = RAFT_STATE_FOLLOWER;
    engine->current_term = 0;
    engine->voted_for = 0;
    engine->log_size = 0;
    engine->commit_index = 0;
    engine->last_applied = 0;
    engine->peer_count = 0;
    engine->vote_count = 0;
    engine->quorum_op_count = 0;
    engine->election_timeout_ms = 300;
    engine->heartbeat_timeout_ms = 150;
    engine->total_log_entries = 0;
    engine->total_elections = 0;
    engine->total_commits = 0;

    global_consensus_engine = engine;
    return engine;
}

int consensus_engine_destroy(ConsensusEngine *engine)
{
    if (!engine)
        return -1;

    for (uint64_t i = 0; i < engine->log_size; i++)
    {
        if (engine->log[i].data)
        {
            free(engine->log[i].data);
        }
    }

    free(engine);
    global_consensus_engine = NULL;
    return 0;
}

int consensus_add_peer(ConsensusEngine *engine, uint32_t peer_id, uint32_t node_id)
{
    if (!engine || engine->peer_count >= MAX_CONSENSUS_PEERS)
        return -1;

    PeerState *peer = &engine->peers[engine->peer_count];
    memset(peer, 0, sizeof(PeerState));

    peer->peer_id = peer_id;
    peer->node_id = node_id;
    peer->next_index = engine->log_size + 1;
    peer->match_index = 0;
    peer->is_active = 1;
    peer->failed_request_count = 0;

    engine->peer_count++;
    return 0;
}

int consensus_remove_peer(ConsensusEngine *engine, uint32_t peer_id)
{
    if (!engine || peer_id == 0)
        return -1;

    for (uint32_t i = 0; i < engine->peer_count; i++)
    {
        if (engine->peers[i].peer_id == peer_id)
        {
            memmove(&engine->peers[i], &engine->peers[i + 1],
                    sizeof(PeerState) * (engine->peer_count - i - 1));
            engine->peer_count--;
            return 0;
        }
    }

    return -1;
}

int consensus_append_log_entry(ConsensusEngine *engine, ConsensusOperationType op_type,
                               const uint8_t *data, uint32_t data_size)
{
    if (!engine || engine->log_size >= MAX_LOG_ENTRIES)
        return -1;

    LogEntry *entry = &engine->log[engine->log_size];
    memset(entry, 0, sizeof(LogEntry));

    entry->log_index = engine->log_size + 1;
    entry->term = engine->current_term;
    entry->op_type = op_type;
    entry->timestamp = 0;

    if (data && data_size > 0)
    {
        entry->data = (uint8_t *)malloc(data_size);
        if (!entry->data)
            return -1;
        memcpy(entry->data, data, data_size);
        entry->data_size = data_size;
    }

    engine->log_size++;
    engine->total_log_entries++;
    return entry->log_index;
}

int consensus_commit_entry(ConsensusEngine *engine, uint64_t log_index)
{
    if (!engine || log_index == 0 || log_index > engine->log_size)
        return -1;

    engine->commit_index = log_index;
    engine->total_commits++;
    return 0;
}

int consensus_request_vote(ConsensusEngine *engine, uint32_t term, uint32_t candidate_id)
{
    if (!engine || term == 0 || candidate_id == 0)
        return -1;

    if (term > engine->current_term)
    {
        engine->current_term = term;
        engine->voted_for = 0;
    }

    if (engine->voted_for == 0 || engine->voted_for == candidate_id)
    {
        if (term >= engine->current_term)
        {
            engine->voted_for = candidate_id;
            engine->vote_count++;
            return 0;
        }
    }

    return -1;
}

int consensus_grant_vote(ConsensusEngine *engine, uint32_t from_peer_id)
{
    if (!engine || from_peer_id == 0)
        return -1;

    for (uint32_t i = 0; i < engine->peer_count; i++)
    {
        if (engine->peers[i].peer_id == from_peer_id)
        {
            return 0;
        }
    }

    return -1;
}

int consensus_append_entries(ConsensusEngine *engine, uint32_t term, uint32_t leader_id,
                             uint64_t prev_log_index, uint32_t prev_log_term)
{
    if (!engine || term == 0 || leader_id == 0)
        return -1;

    if (term > engine->current_term)
    {
        engine->current_term = term;
        engine->voted_for = 0;
        engine->state = RAFT_STATE_FOLLOWER;
    }

    return 0;
}

int consensus_accept_entries(ConsensusEngine *engine, uint32_t from_peer_id)
{
    if (!engine || from_peer_id == 0)
        return -1;

    for (uint32_t i = 0; i < engine->peer_count; i++)
    {
        if (engine->peers[i].peer_id == from_peer_id)
        {
            engine->peers[i].match_index = engine->log_size;
            return 0;
        }
    }

    return -1;
}

int consensus_start_election(ConsensusEngine *engine)
{
    if (!engine)
        return -1;

    engine->current_term++;
    engine->voted_for = engine->node_id;
    engine->state = RAFT_STATE_CANDIDATE;
    engine->vote_count = 1;
    engine->total_elections++;

    return 0;
}

int consensus_win_election(ConsensusEngine *engine)
{
    if (!engine)
        return -1;

    uint32_t majority = (engine->peer_count + 1) / 2 + 1;

    if (engine->vote_count >= majority)
    {
        return consensus_become_leader(engine);
    }

    return -1;
}

int consensus_become_leader(ConsensusEngine *engine)
{
    if (!engine)
        return -1;

    engine->state = RAFT_STATE_LEADER;

    for (uint32_t i = 0; i < engine->peer_count; i++)
    {
        engine->peers[i].next_index = engine->log_size + 1;
        engine->peers[i].match_index = 0;
    }

    return 0;
}

int consensus_create_quorum_operation(ConsensusEngine *engine, ConsensusOperationType op_type,
                                      const uint8_t *data, uint32_t data_size)
{
    if (!engine || engine->quorum_op_count >= MAX_QUORUM_OPERATIONS)
        return -1;

    if (engine->state != RAFT_STATE_LEADER)
        return -1;

    QuorumOperation *qop = &engine->quorum_ops[engine->quorum_op_count];
    memset(qop, 0, sizeof(QuorumOperation));

    qop->quorum_op_id = next_quorum_op_id++;
    qop->op_type = op_type;
    qop->peer_count = engine->peer_count;

    for (uint32_t i = 0; i < engine->peer_count; i++)
    {
        memcpy(&qop->peers[i], &engine->peers[i], sizeof(PeerState));
    }

    qop->commit_count = 1;
    qop->required_commits = (engine->peer_count + 1) / 2 + 1;
    qop->timestamp = 0;
    qop->is_committed = 0;

    engine->quorum_op_count++;
    return qop->quorum_op_id;
}

int consensus_quorum_operation_complete(ConsensusEngine *engine, uint32_t quorum_op_id)
{
    if (!engine || quorum_op_id == 0)
        return -1;

    for (uint32_t i = 0; i < engine->quorum_op_count; i++)
    {
        if (engine->quorum_ops[i].quorum_op_id == quorum_op_id)
        {
            engine->quorum_ops[i].is_committed = 1;
            return 0;
        }
    }

    return -1;
}

int consensus_apply_committed_entries(ConsensusEngine *engine)
{
    if (!engine)
        return -1;

    uint32_t applied_count = 0;

    while (engine->last_applied < engine->commit_index && engine->last_applied < engine->log_size)
    {
        engine->last_applied++;
        applied_count++;
    }

    return applied_count;
}

int consensus_create_snapshot(ConsensusEngine *engine, uint64_t last_included_index)
{
    if (!engine || last_included_index == 0 || last_included_index > engine->log_size)
        return -1;

    return 0;
}

int consensus_get_metrics(ConsensusEngine *engine, ConsensusMetrics *metrics)
{
    if (!engine || !metrics)
        return -1;

    metrics->engine_id = engine->engine_id;
    metrics->current_state = engine->state;
    metrics->current_term = engine->current_term;
    metrics->log_size = engine->log_size;
    metrics->commit_index = engine->commit_index;
    metrics->peer_count = engine->peer_count;
    metrics->election_count = engine->total_elections;
    metrics->commit_count = engine->total_commits;

    if (engine->peer_count > 0)
    {
        uint32_t active_peers = 0;
        for (uint32_t i = 0; i < engine->peer_count; i++)
        {
            if (engine->peers[i].is_active)
            {
                active_peers++;
            }
        }
        metrics->replication_factor = (float)active_peers / engine->peer_count;
    }
    else
    {
        metrics->replication_factor = 0.0f;
    }

    return 0;
}
