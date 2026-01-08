#ifndef CONSENSUS_ENGINE_H
#define CONSENSUS_ENGINE_H

#include <stdint.h>
#include <stddef.h>

#define MAX_CONSENSUS_PEERS 64
#define MAX_LOG_ENTRIES 16384
#define MAX_VOTE_ENTRIES 256
#define MAX_QUORUM_OPERATIONS 512

typedef enum
{
    RAFT_STATE_FOLLOWER,
    RAFT_STATE_CANDIDATE,
    RAFT_STATE_LEADER
} RaftState;

typedef enum
{
    CONSENSUS_OPERATION_APPEND,
    CONSENSUS_OPERATION_COMMIT,
    CONSENSUS_OPERATION_SNAPSHOT,
    CONSENSUS_OPERATION_CONFIG_CHANGE
} ConsensusOperationType;

typedef struct
{
    uint32_t term;
    uint32_t vote_for;
    uint8_t voted;
} VoteEntry;

typedef struct
{
    uint32_t log_index;
    uint32_t term;
    ConsensusOperationType op_type;

    uint8_t *data;
    uint32_t data_size;
    uint64_t timestamp;
} LogEntry;

typedef struct
{
    uint32_t peer_id;
    uint32_t node_id;
    uint64_t next_index;
    uint64_t match_index;

    uint8_t is_active;
    uint32_t failed_request_count;
} PeerState;

typedef struct
{
    uint32_t quorum_op_id;
    ConsensusOperationType op_type;

    PeerState peers[MAX_CONSENSUS_PEERS];
    uint32_t peer_count;

    uint32_t commit_count;
    uint32_t required_commits;

    uint64_t timestamp;
    uint8_t is_committed;
} QuorumOperation;

typedef struct
{
    uint32_t engine_id;
    uint32_t node_id;

    RaftState state;
    uint32_t current_term;
    uint32_t voted_for;

    LogEntry log[MAX_LOG_ENTRIES];
    uint64_t log_size;
    uint64_t commit_index;
    uint64_t last_applied;

    PeerState peers[MAX_CONSENSUS_PEERS];
    uint32_t peer_count;

    VoteEntry votes[MAX_VOTE_ENTRIES];
    uint32_t vote_count;

    QuorumOperation quorum_ops[MAX_QUORUM_OPERATIONS];
    uint32_t quorum_op_count;

    uint32_t election_timeout_ms;
    uint32_t heartbeat_timeout_ms;

    uint64_t total_log_entries;
    uint32_t total_elections;
    uint32_t total_commits;
} ConsensusEngine;

ConsensusEngine *consensus_engine_init(uint32_t node_id);
int consensus_engine_destroy(ConsensusEngine *engine);

int consensus_add_peer(ConsensusEngine *engine, uint32_t peer_id, uint32_t node_id);
int consensus_remove_peer(ConsensusEngine *engine, uint32_t peer_id);

int consensus_append_log_entry(ConsensusEngine *engine, ConsensusOperationType op_type,
                               const uint8_t *data, uint32_t data_size);
int consensus_commit_entry(ConsensusEngine *engine, uint64_t log_index);

int consensus_request_vote(ConsensusEngine *engine, uint32_t term, uint32_t candidate_id);
int consensus_grant_vote(ConsensusEngine *engine, uint32_t from_peer_id);

int consensus_append_entries(ConsensusEngine *engine, uint32_t term, uint32_t leader_id,
                             uint64_t prev_log_index, uint32_t prev_log_term);
int consensus_accept_entries(ConsensusEngine *engine, uint32_t from_peer_id);

int consensus_start_election(ConsensusEngine *engine);
int consensus_win_election(ConsensusEngine *engine);
int consensus_become_leader(ConsensusEngine *engine);

int consensus_create_quorum_operation(ConsensusEngine *engine, ConsensusOperationType op_type,
                                      const uint8_t *data, uint32_t data_size);
int consensus_quorum_operation_complete(ConsensusEngine *engine, uint32_t quorum_op_id);

int consensus_apply_committed_entries(ConsensusEngine *engine);
int consensus_create_snapshot(ConsensusEngine *engine, uint64_t last_included_index);

typedef struct
{
    uint32_t engine_id;
    RaftState current_state;
    uint32_t current_term;
    uint64_t log_size;
    uint64_t commit_index;
    uint32_t peer_count;
    uint32_t election_count;
    uint32_t commit_count;
    float replication_factor;
} ConsensusMetrics;

int consensus_get_metrics(ConsensusEngine *engine, ConsensusMetrics *metrics);

#endif
