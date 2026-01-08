#ifndef POWER_MANAGER_H
#define POWER_MANAGER_H

#include <stdint.h>
#include <stddef.h>

#define MAX_POWER_DOMAINS 128
#define MAX_POWER_STATES 8
#define MAX_POWER_TRANSITIONS 512

typedef enum
{
    POWER_STATE_ACTIVE,
    POWER_STATE_IDLE,
    POWER_STATE_SLEEP,
    POWER_STATE_DEEP_SLEEP,
    POWER_STATE_HIBERNATION
} PowerState;

typedef enum
{
    POWER_POLICY_PERFORMANCE,
    POWER_POLICY_BALANCED,
    POWER_POLICY_POWER_SAVING
} PowerPolicy;

typedef struct
{
    uint32_t domain_id;
    char domain_name[64];

    PowerState current_state;
    PowerState target_state;

    uint64_t active_duration_us;
    uint64_t idle_duration_us;
    uint64_t power_consumption_mw;

    uint32_t state_transition_count;
    uint8_t can_transition;
} PowerDomain;

typedef struct
{
    uint32_t transition_id;
    uint32_t from_domain_id;
    uint32_t to_domain_id;

    PowerState from_state;
    PowerState to_state;

    uint64_t transition_time_us;
    uint64_t transition_timestamp;
} PowerTransition;

typedef struct
{
    uint32_t manager_id;
    PowerDomain domains[MAX_POWER_DOMAINS];
    uint32_t domain_count;

    PowerTransition transitions[MAX_POWER_TRANSITIONS];
    uint32_t transition_count;

    PowerPolicy policy;
    uint64_t total_power_saved_mwh;
    uint32_t idle_transitions;
    uint32_t sleep_transitions;
    float average_power_consumption_mw;
} PowerManager;

PowerManager *power_manager_init(PowerPolicy policy);
int power_manager_destroy(PowerManager *manager);

uint32_t power_domain_register(PowerManager *manager, const char *domain_name);
int power_domain_unregister(PowerManager *manager, uint32_t domain_id);

int power_domain_set_state(PowerManager *manager, uint32_t domain_id, PowerState state);
int power_domain_get_state(PowerManager *manager, uint32_t domain_id, PowerState *state);

int power_enable_idle_state(PowerManager *manager, uint32_t domain_id);
int power_enable_sleep_state(PowerManager *manager, uint32_t domain_id);

int power_transition_domain(PowerManager *manager, uint32_t from_domain_id,
                            uint32_t to_domain_id, PowerState to_state);
int power_record_transition(PowerManager *manager, uint32_t from_id, uint32_t to_id,
                            PowerState from_state, PowerState to_state);

int power_policy_set(PowerManager *manager, PowerPolicy policy);
int power_policy_get(PowerManager *manager, PowerPolicy *policy);

int power_calculate_consumption(PowerManager *manager);
int power_optimize_consumption(PowerManager *manager);

int power_enable_dvfs(PowerManager *manager, uint32_t domain_id);
int power_disable_dvfs(PowerManager *manager, uint32_t domain_id);

typedef struct
{
    uint32_t manager_id;
    uint32_t total_domains;
    uint32_t active_domains;
    uint32_t idle_domains;
    uint32_t sleeping_domains;
    uint64_t total_power_saved_mwh;
    uint32_t transitions;
    float average_power_mw;
} PowerManagerMetrics;

int power_manager_get_metrics(PowerManager *manager, PowerManagerMetrics *metrics);

#endif
