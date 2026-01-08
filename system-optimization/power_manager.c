#include "power_manager.h"
#include <stdlib.h>
#include <string.h>

static PowerManager *global_power_manager = NULL;
static uint32_t next_domain_id = 1;
static uint32_t next_transition_id = 1;

PowerManager *power_manager_init(PowerPolicy policy)
{
    PowerManager *manager = (PowerManager *)malloc(sizeof(PowerManager));
    if (!manager)
        return NULL;

    memset(manager, 0, sizeof(PowerManager));
    manager->manager_id = 1;
    manager->domain_count = 0;
    manager->transition_count = 0;
    manager->policy = policy;
    manager->total_power_saved_mwh = 0;
    manager->idle_transitions = 0;
    manager->sleep_transitions = 0;
    manager->average_power_consumption_mw = 0.0f;

    global_power_manager = manager;
    return manager;
}

int power_manager_destroy(PowerManager *manager)
{
    if (!manager)
        return -1;

    free(manager);
    global_power_manager = NULL;
    return 0;
}

uint32_t power_domain_register(PowerManager *manager, const char *domain_name)
{
    if (!manager || manager->domain_count >= MAX_POWER_DOMAINS)
        return 0;

    PowerDomain *domain = &manager->domains[manager->domain_count];
    memset(domain, 0, sizeof(PowerDomain));

    domain->domain_id = next_domain_id++;

    if (domain_name)
    {
        strncpy(domain->domain_name, domain_name, sizeof(domain->domain_name) - 1);
    }

    domain->current_state = POWER_STATE_ACTIVE;
    domain->target_state = POWER_STATE_ACTIVE;
    domain->active_duration_us = 0;
    domain->idle_duration_us = 0;
    domain->power_consumption_mw = 500;
    domain->state_transition_count = 0;
    domain->can_transition = 1;

    manager->domain_count++;
    return domain->domain_id;
}

int power_domain_unregister(PowerManager *manager, uint32_t domain_id)
{
    if (!manager || domain_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->domain_count; i++)
    {
        if (manager->domains[i].domain_id == domain_id)
        {
            memmove(&manager->domains[i], &manager->domains[i + 1],
                    sizeof(PowerDomain) * (manager->domain_count - i - 1));
            manager->domain_count--;
            return 0;
        }
    }

    return -1;
}

int power_domain_set_state(PowerManager *manager, uint32_t domain_id, PowerState state)
{
    if (!manager || domain_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->domain_count; i++)
    {
        if (manager->domains[i].domain_id == domain_id)
        {
            PowerDomain *domain = &manager->domains[i];

            if (!domain->can_transition)
                return -1;

            domain->target_state = state;
            power_transition_domain(manager, domain_id, domain_id, state);

            return 0;
        }
    }

    return -1;
}

int power_domain_get_state(PowerManager *manager, uint32_t domain_id, PowerState *state)
{
    if (!manager || domain_id == 0 || !state)
        return -1;

    for (uint32_t i = 0; i < manager->domain_count; i++)
    {
        if (manager->domains[i].domain_id == domain_id)
        {
            *state = manager->domains[i].current_state;
            return 0;
        }
    }

    return -1;
}

int power_enable_idle_state(PowerManager *manager, uint32_t domain_id)
{
    if (!manager || domain_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->domain_count; i++)
    {
        if (manager->domains[i].domain_id == domain_id)
        {
            PowerDomain *domain = &manager->domains[i];

            if (domain->current_state == POWER_STATE_ACTIVE)
            {
                domain->target_state = POWER_STATE_IDLE;
                manager->idle_transitions++;
                domain->idle_duration_us += 1000000;
                return 0;
            }
        }
    }

    return -1;
}

int power_enable_sleep_state(PowerManager *manager, uint32_t domain_id)
{
    if (!manager || domain_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->domain_count; i++)
    {
        if (manager->domains[i].domain_id == domain_id)
        {
            PowerDomain *domain = &manager->domains[i];

            if (domain->current_state != POWER_STATE_HIBERNATION)
            {
                domain->target_state = POWER_STATE_SLEEP;
                manager->sleep_transitions++;
                domain->idle_duration_us += 5000000;
                return 0;
            }
        }
    }

    return -1;
}

int power_transition_domain(PowerManager *manager, uint32_t from_domain_id,
                            uint32_t to_domain_id, PowerState to_state)
{
    if (!manager || from_domain_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->domain_count; i++)
    {
        if (manager->domains[i].domain_id == from_domain_id)
        {
            PowerDomain *domain = &manager->domains[i];
            PowerState from_state = domain->current_state;
            domain->current_state = to_state;
            domain->state_transition_count++;

            power_record_transition(manager, from_domain_id, to_domain_id, from_state, to_state);
            return 0;
        }
    }

    return -1;
}

int power_record_transition(PowerManager *manager, uint32_t from_id, uint32_t to_id,
                            PowerState from_state, PowerState to_state)
{
    if (!manager || manager->transition_count >= MAX_POWER_TRANSITIONS)
        return -1;

    PowerTransition *trans = &manager->transitions[manager->transition_count];
    memset(trans, 0, sizeof(PowerTransition));

    trans->transition_id = next_transition_id++;
    trans->from_domain_id = from_id;
    trans->to_domain_id = to_id;
    trans->from_state = from_state;
    trans->to_state = to_state;
    trans->transition_time_us = 1000;
    trans->transition_timestamp = 0;

    manager->transition_count++;
    return trans->transition_id;
}

int power_policy_set(PowerManager *manager, PowerPolicy policy)
{
    if (!manager)
        return -1;

    manager->policy = policy;
    return 0;
}

int power_policy_get(PowerManager *manager, PowerPolicy *policy)
{
    if (!manager || !policy)
        return -1;

    *policy = manager->policy;
    return 0;
}

int power_calculate_consumption(PowerManager *manager)
{
    if (!manager)
        return -1;

    uint64_t total_power = 0;

    for (uint32_t i = 0; i < manager->domain_count; i++)
    {
        PowerDomain *domain = &manager->domains[i];

        uint64_t adjusted_power = domain->power_consumption_mw;

        if (domain->current_state == POWER_STATE_IDLE)
        {
            adjusted_power = (adjusted_power * 50) / 100;
        }
        else if (domain->current_state == POWER_STATE_SLEEP)
        {
            adjusted_power = (adjusted_power * 10) / 100;
        }
        else if (domain->current_state == POWER_STATE_DEEP_SLEEP)
        {
            adjusted_power = (adjusted_power * 2) / 100;
        }

        total_power += adjusted_power;
    }

    if (manager->domain_count > 0)
    {
        manager->average_power_consumption_mw = (float)total_power / manager->domain_count;
    }

    return 0;
}

int power_optimize_consumption(PowerManager *manager)
{
    if (!manager)
        return -1;

    for (uint32_t i = 0; i < manager->domain_count; i++)
    {
        PowerDomain *domain = &manager->domains[i];

        if (manager->policy == POWER_POLICY_POWER_SAVING)
        {
            if (domain->current_state == POWER_STATE_ACTIVE)
            {
                power_enable_idle_state(manager, domain->domain_id);
                manager->total_power_saved_mwh += 100;
            }
        }
    }

    return 0;
}

int power_enable_dvfs(PowerManager *manager, uint32_t domain_id)
{
    if (!manager || domain_id == 0)
        return -1;

    return 0;
}

int power_disable_dvfs(PowerManager *manager, uint32_t domain_id)
{
    if (!manager || domain_id == 0)
        return -1;

    return 0;
}

int power_manager_get_metrics(PowerManager *manager, PowerManagerMetrics *metrics)
{
    if (!manager || !metrics)
        return -1;

    uint32_t active = 0, idle = 0, sleeping = 0;

    for (uint32_t i = 0; i < manager->domain_count; i++)
    {
        if (manager->domains[i].current_state == POWER_STATE_ACTIVE)
            active++;
        else if (manager->domains[i].current_state == POWER_STATE_IDLE)
            idle++;
        else if (manager->domains[i].current_state == POWER_STATE_SLEEP ||
                 manager->domains[i].current_state == POWER_STATE_DEEP_SLEEP)
            sleeping++;
    }

    metrics->manager_id = manager->manager_id;
    metrics->total_domains = manager->domain_count;
    metrics->active_domains = active;
    metrics->idle_domains = idle;
    metrics->sleeping_domains = sleeping;
    metrics->total_power_saved_mwh = manager->total_power_saved_mwh;
    metrics->transitions = manager->transition_count;
    metrics->average_power_mw = manager->average_power_consumption_mw;

    return 0;
}
