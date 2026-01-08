#ifndef DISTRIBUTED_CONSENSUS_H
#define DISTRIBUTED_CONSENSUS_H

#include <stdint.h>
#include <stddef.h>

#define MAX_CONSENSUS_NODES 256
#define MAX_LOG_ENTRIES 10000
#define MAX_STATE_MACHINES 32
#define CONSENSUS_TIMEOUT_MS 1000

typedef enum
{
    CONSENSUS_FOLLOWER,
    CONSENSUS_CANDIDATE,
    CONSENSUS_LEADER,
    CONSENSUS_DEAD
} ConsensusState;

typedef enum
{
    LOG_ENTRY_CLIENT_REQUEST,
    LOG_ENTRY_MEMBERSHIP_CHANGE,
    LOG_ENTRY_SNAPSHOT,
    LOG_ENTRY_CONFIGURATION
} LogEntryType;

typedef struct
{
    uint64_t term;
    uint64_t index;
    LogEntryType type;

    uint32_t client_id;
    uint32_t command_id;

    uint64_t data_size;
    uint8_t *data;

    uint32_t timestamp;
    uint8_t replicated;
} LogEntry;

typedef struct
{
    uint32_t server_id;
    char server_address[64];
    uint16_t port;

    uint64_t next_index;
    uint64_t match_index;
    uint8_t is_leader;
} ServerState;

typedef struct
{
    uint32_t node_id;
    char node_address[64];
    uint16_t port;

    // Persistent state
    uint64_t current_term;
    uint32_t voted_for;
    LogEntry *log_entries;
    uint64_t log_size;

    // Volatile state
    uint64_t commit_index;
    uint64_t last_applied;

    // Leader only
    uint64_t *next_index;
    uint64_t *match_index;

    // State
    ConsensusState state;
    uint32_t election_timeout;
    uint32_t last_heartbeat;
} ConsensusNode;

typedef struct
{
    uint32_t consensus_id;

    uint32_t node_count;
    ConsensusNode nodes[MAX_CONSENSUS_NODES];

    uint32_t leader_id;
    uint64_t current_term;

    LogEntry log_entries[MAX_LOG_ENTRIES];
    uint64_t log_size;
    uint64_t log_applied;

    uint64_t total_entries_replicated;
    uint64_t total_entries_committed;
    uint64_t total_elections;
    uint64_t last_election_time;
} ConsensusManager;

// Consensus Manager initialization
ConsensusManager *consensus_manager_init(uint32_t node_id, uint32_t total_nodes);
int consensus_manager_destroy(ConsensusManager *cm);

// Node management
int consensus_add_node(ConsensusManager *cm, uint32_t node_id, const char *address, uint16_t port);
int consensus_remove_node(ConsensusManager *cm, uint32_t node_id);

// State transitions
int consensus_become_follower(ConsensusNode *node, uint64_t term);
int consensus_become_candidate(ConsensusNode *node);
int consensus_become_leader(ConsensusNode *node);

// Log operations
int consensus_append_entry(ConsensusNode *node, LogEntryType type, const uint8_t *data, uint64_t data_size);
int consensus_replicate_entry(ConsensusManager *cm, uint64_t log_index);
int consensus_commit_entries(ConsensusManager *cm);

// RPC operations (simplified)
typedef struct
{
    uint64_t term;
    uint32_t leader_id;
    uint64_t prev_log_index;
    uint64_t prev_log_term;
    uint64_t leader_commit;
    LogEntry *entries;
    uint64_t entry_count;
} AppendEntriesRequest;

typedef struct
{
    uint64_t term;
    uint8_t success;
    uint64_t match_index;
} AppendEntriesResponse;

int consensus_append_entries_handler(ConsensusNode *node, AppendEntriesRequest *request, AppendEntriesResponse *response);

typedef struct
{
    uint64_t term;
    uint64_t last_log_index;
    uint64_t last_log_term;
} RequestVoteRequest;

typedef struct
{
    uint64_t term;
    uint8_t vote_granted;
} RequestVoteResponse;

int consensus_request_vote_handler(ConsensusNode *node, RequestVoteRequest *request, RequestVoteResponse *response);

// Election and leader operations
int consensus_start_election(ConsensusNode *node);
int consensus_handle_election_timeout(ConsensusManager *cm);
int consensus_process_client_request(ConsensusManager *cm, uint32_t client_id, const uint8_t *command, uint64_t command_size);

// Snapshot and compaction
typedef struct
{
    uint64_t index;
    uint64_t term;
    uint64_t snapshot_size;
    uint8_t *snapshot_data;
    uint32_t snapshot_timestamp;
} Snapshot;

int consensus_create_snapshot(ConsensusNode *node);
int consensus_restore_snapshot(ConsensusNode *node, Snapshot *snapshot);
int consensus_compact_log(ConsensusNode *node, uint64_t retain_entries);

// Monitoring
typedef struct
{
    uint32_t node_id;
    ConsensusState current_state;
    uint64_t current_term;
    uint64_t log_size;
    uint64_t last_applied;
    uint64_t entries_replicated;
    float replication_latency_ms;
} ConsensusMetrics;

int consensus_get_metrics(ConsensusNode *node, ConsensusMetrics *metrics);
int consensus_manager_get_metrics(ConsensusManager *cm);

// Membership changes
int consensus_add_node_to_cluster(ConsensusManager *cm, const char *address, uint16_t port);
int consensus_remove_node_from_cluster(ConsensusManager *cm, uint32_t node_id);

#endif // DISTRIBUTED_CONSENSUS_H
