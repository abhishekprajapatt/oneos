#ifndef ONEOS_INTELLIGENCE_COORDINATOR_H
#define ONEOS_INTELLIGENCE_COORDINATOR_H

#include <stdint.h>
#include <time.h>

#define MAX_INTELLIGENCE_COORDINATORS 16
#define MAX_AI_DECISIONS 512
#define MAX_SUBSYSTEM_AGENTS 64
#define MAX_DECISION_RULES 256
#define MAX_LEARNING_SAMPLES 2048

typedef enum
{
    SUBSYSTEM_PREDICTIVE_OPTIMIZER = 0,
    SUBSYSTEM_RESOURCE_MANAGER = 1,
    SUBSYSTEM_HEALING_MANAGER = 2,
    SUBSYSTEM_SLA_MANAGER = 3,
    SUBSYSTEM_COST_OPTIMIZER = 4,
    SUBSYSTEM_WORKLOAD_MANAGER = 5,
    SUBSYSTEM_SCHEDULER = 6,
    SUBSYSTEM_SECURITY = 7
} SubsystemType;

typedef enum
{
    DECISION_TYPE_RESOURCE_ALLOCATION = 0,
    DECISION_TYPE_SCALING = 1,
    DECISION_TYPE_WORKLOAD_DISTRIBUTION = 2,
    DECISION_TYPE_FAULT_RECOVERY = 3,
    DECISION_TYPE_COST_OPTIMIZATION = 4,
    DECISION_TYPE_PERFORMANCE_TUNING = 5,
    DECISION_TYPE_SECURITY_ACTION = 6,
    DECISION_TYPE_SLA_ASSURANCE = 7
} DecisionType;

typedef enum
{
    DECISION_STATE_PROPOSED = 0,
    DECISION_STATE_APPROVED = 1,
    DECISION_STATE_EXECUTING = 2,
    DECISION_STATE_EXECUTED = 3,
    DECISION_STATE_VERIFIED = 4,
    DECISION_STATE_FAILED = 5,
    DECISION_STATE_REVERTED = 6
} DecisionState;

typedef enum
{
    CONFLICT_RESOLUTION_PRIORITY = 0,
    CONFLICT_RESOLUTION_WEIGHTED_SCORE = 1,
    CONFLICT_RESOLUTION_TIME_BASED = 2,
    CONFLICT_RESOLUTION_IMPACT_ANALYSIS = 3,
    CONFLICT_RESOLUTION_CONSENSUS = 4
} ConflictResolutionStrategy;

typedef enum
{
    LEARNING_STRATEGY_REINFORCEMENT = 0,
    LEARNING_STRATEGY_SUPERVISED = 1,
    LEARNING_STRATEGY_UNSUPERVISED = 2,
    LEARNING_STRATEGY_MULTI_AGENT = 3
} LearningStrategy;

typedef struct
{
    uint32_t agent_id;
    SubsystemType subsystem_type;
    char agent_name[64];
    uint32_t decisions_proposed;
    uint32_t decisions_approved;
    uint32_t decisions_successful;
    double success_rate;
    uint8_t is_active;
} SubsystemAgent;

typedef struct
{
    uint64_t decision_id;
    DecisionType decision_type;
    SubsystemType origin_subsystem;
    char decision_description[256];
    DecisionState current_state;
    uint32_t priority;
    double confidence_score;
    time_t creation_time;
    time_t execution_time;
    time_t verification_time;
    int32_t outcome_score;
    uint8_t is_reversible;
} AIDecision;

typedef struct
{
    uint32_t rule_id;
    DecisionType decision_type;
    char rule_name[64];
    char condition_expression[256];
    char recommended_action[256];
    uint32_t priority;
    uint32_t applications;
    double effectiveness_rate;
    uint8_t is_enabled;
} DecisionRule;

typedef struct
{
    uint32_t sample_id;
    DecisionType decision_type;
    uint64_t state_hash;
    char input_features[512];
    char executed_action[256];
    int32_t outcome_score;
    double reward;
    time_t timestamp;
} LearningSample;

typedef struct
{
    uint32_t coordinator_id;
    char name[64];

    uint32_t agent_count;
    SubsystemAgent subsystem_agents[64];

    uint32_t decision_count;
    AIDecision ai_decisions[512];

    uint32_t rule_count;
    DecisionRule decision_rules[256];

    uint32_t sample_count;
    LearningSample learning_samples[2048];

    ConflictResolutionStrategy conflict_strategy;
    LearningStrategy learning_strategy;

    uint64_t total_decisions_made;
    uint64_t decisions_approved;
    uint64_t decisions_executed;
    uint64_t decisions_successful;
    double average_confidence_score;
    double average_outcome_score;

    uint32_t conflicts_detected;
    uint32_t conflicts_resolved;
    uint32_t learning_iterations;

    time_t created_at;
    time_t last_optimization;
    uint8_t is_active;
} IntelligenceCoordinator;

int ic_create_coordinator(const char *name);
int ic_delete_coordinator(uint32_t coordinator_id);

int ic_register_subsystem_agent(uint32_t coordinator_id, SubsystemType subsystem_type,
                                const char *agent_name);
int ic_unregister_subsystem_agent(uint32_t coordinator_id, uint32_t agent_id);
int ic_enable_subsystem_agent(uint32_t coordinator_id, uint32_t agent_id);
int ic_disable_subsystem_agent(uint32_t coordinator_id, uint32_t agent_id);

int ic_propose_decision(uint32_t coordinator_id, DecisionType decision_type,
                        SubsystemType origin_subsystem, const char *description,
                        uint32_t priority, double confidence);
int ic_approve_decision(uint32_t coordinator_id, uint64_t decision_id);
int ic_reject_decision(uint32_t coordinator_id, uint64_t decision_id);
int ic_execute_decision(uint32_t coordinator_id, uint64_t decision_id);
int ic_verify_decision_outcome(uint32_t coordinator_id, uint64_t decision_id, int32_t outcome);
int ic_revert_decision(uint32_t coordinator_id, uint64_t decision_id);

int ic_add_decision_rule(uint32_t coordinator_id, DecisionType decision_type,
                         const char *rule_name, const char *condition,
                         const char *recommended_action, uint32_t priority);
int ic_remove_decision_rule(uint32_t coordinator_id, uint32_t rule_id);
int ic_enable_decision_rule(uint32_t coordinator_id, uint32_t rule_id);
int ic_disable_decision_rule(uint32_t coordinator_id, uint32_t rule_id);

int ic_detect_decision_conflict(uint32_t coordinator_id, const AIDecision *decision1,
                                const AIDecision *decision2);
int ic_resolve_conflict(uint32_t coordinator_id, uint64_t decision_id1,
                        uint64_t decision_id2, ConflictResolutionStrategy strategy);

int ic_record_learning_sample(uint32_t coordinator_id, DecisionType decision_type,
                              const char *input_features, const char *action,
                              int32_t outcome, double reward);
int ic_perform_learning_iteration(uint32_t coordinator_id, LearningStrategy strategy);
int ic_update_decision_rules_from_learning(uint32_t coordinator_id);

int ic_orchestrate_system_optimization(uint32_t coordinator_id);
int ic_get_optimal_decision_path(uint32_t coordinator_id, DecisionType decision_type,
                                 uint64_t *decision_ids, uint32_t *count);

double ic_get_system_intelligence_score(uint32_t coordinator_id);
int ic_get_subsystem_agent_recommendations(uint32_t coordinator_id,
                                           SubsystemType subsystem_type,
                                           AIDecision *recommendations, uint32_t *count);

IntelligenceCoordinator *ic_get_coordinator_info(uint32_t coordinator_id);

#endif
