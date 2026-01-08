#include "intelligence_coordinator.h"
#include <stdlib.h>
#include <string.h>

static IntelligenceCoordinator g_coordinators[MAX_INTELLIGENCE_COORDINATORS] = {0};
static uint32_t g_coordinator_count = 0;

int ic_create_coordinator(const char *name)
{
    if (g_coordinator_count >= MAX_INTELLIGENCE_COORDINATORS)
    {
        return -1;
    }

    IntelligenceCoordinator *coord = &g_coordinators[g_coordinator_count];
    coord->coordinator_id = g_coordinator_count++;
    strncpy(coord->name, name, 63);
    coord->name[63] = '\0';

    coord->agent_count = 0;
    coord->decision_count = 0;
    coord->rule_count = 0;
    coord->sample_count = 0;

    coord->conflict_strategy = CONFLICT_RESOLUTION_WEIGHTED_SCORE;
    coord->learning_strategy = LEARNING_STRATEGY_REINFORCEMENT;

    coord->total_decisions_made = 0;
    coord->decisions_approved = 0;
    coord->decisions_executed = 0;
    coord->decisions_successful = 0;
    coord->average_confidence_score = 0.0;
    coord->average_outcome_score = 0.0;

    coord->conflicts_detected = 0;
    coord->conflicts_resolved = 0;
    coord->learning_iterations = 0;

    coord->created_at = time(NULL);
    coord->last_optimization = 0;
    coord->is_active = 1;

    return coord->coordinator_id;
}

int ic_delete_coordinator(uint32_t coordinator_id)
{
    if (coordinator_id >= g_coordinator_count)
    {
        return -1;
    }

    IntelligenceCoordinator *coord = &g_coordinators[coordinator_id];
    coord->is_active = 0;
    coord->agent_count = 0;
    coord->decision_count = 0;
    coord->rule_count = 0;

    return 0;
}

int ic_register_subsystem_agent(uint32_t coordinator_id, SubsystemType subsystem_type,
                                const char *agent_name)
{
    if (coordinator_id >= g_coordinator_count)
    {
        return -1;
    }

    IntelligenceCoordinator *coord = &g_coordinators[coordinator_id];
    if (coord->agent_count >= MAX_SUBSYSTEM_AGENTS)
    {
        return -1;
    }

    SubsystemAgent *agent = &coord->subsystem_agents[coord->agent_count];
    agent->agent_id = coord->agent_count;
    agent->subsystem_type = subsystem_type;
    strncpy(agent->agent_name, agent_name, 63);
    agent->agent_name[63] = '\0';
    agent->decisions_proposed = 0;
    agent->decisions_approved = 0;
    agent->decisions_successful = 0;
    agent->success_rate = 0.0;
    agent->is_active = 1;

    coord->agent_count++;

    return agent->agent_id;
}

int ic_unregister_subsystem_agent(uint32_t coordinator_id, uint32_t agent_id)
{
    if (coordinator_id >= g_coordinator_count)
    {
        return -1;
    }

    IntelligenceCoordinator *coord = &g_coordinators[coordinator_id];
    if (agent_id >= coord->agent_count)
    {
        return -1;
    }

    coord->subsystem_agents[agent_id].is_active = 0;

    return 0;
}

int ic_enable_subsystem_agent(uint32_t coordinator_id, uint32_t agent_id)
{
    if (coordinator_id >= g_coordinator_count)
    {
        return -1;
    }

    IntelligenceCoordinator *coord = &g_coordinators[coordinator_id];
    if (agent_id >= coord->agent_count)
    {
        return -1;
    }

    coord->subsystem_agents[agent_id].is_active = 1;

    return 0;
}

int ic_disable_subsystem_agent(uint32_t coordinator_id, uint32_t agent_id)
{
    if (coordinator_id >= g_coordinator_count)
    {
        return -1;
    }

    IntelligenceCoordinator *coord = &g_coordinators[coordinator_id];
    if (agent_id >= coord->agent_count)
    {
        return -1;
    }

    coord->subsystem_agents[agent_id].is_active = 0;

    return 0;
}

int ic_propose_decision(uint32_t coordinator_id, DecisionType decision_type,
                        SubsystemType origin_subsystem, const char *description,
                        uint32_t priority, double confidence)
{
    if (coordinator_id >= g_coordinator_count)
    {
        return -1;
    }

    IntelligenceCoordinator *coord = &g_coordinators[coordinator_id];
    if (coord->decision_count >= MAX_AI_DECISIONS)
    {
        return -1;
    }

    AIDecision *decision = &coord->ai_decisions[coord->decision_count];
    decision->decision_id = coord->decision_count;
    decision->decision_type = decision_type;
    decision->origin_subsystem = origin_subsystem;
    strncpy(decision->decision_description, description, 255);
    decision->decision_description[255] = '\0';
    decision->current_state = DECISION_STATE_PROPOSED;
    decision->priority = priority;
    decision->confidence_score = confidence;
    decision->creation_time = time(NULL);
    decision->execution_time = 0;
    decision->verification_time = 0;
    decision->outcome_score = 0;
    decision->is_reversible = 1;

    coord->decision_count++;
    coord->total_decisions_made++;

    return decision->decision_id;
}

int ic_approve_decision(uint32_t coordinator_id, uint64_t decision_id)
{
    if (coordinator_id >= g_coordinator_count)
    {
        return -1;
    }

    IntelligenceCoordinator *coord = &g_coordinators[coordinator_id];
    if (decision_id >= coord->decision_count)
    {
        return -1;
    }

    AIDecision *decision = &coord->ai_decisions[decision_id];
    decision->current_state = DECISION_STATE_APPROVED;
    coord->decisions_approved++;

    if (decision->origin_subsystem < MAX_SUBSYSTEM_AGENTS)
    {
        coord->subsystem_agents[decision->origin_subsystem].decisions_approved++;
    }

    return 0;
}

int ic_reject_decision(uint32_t coordinator_id, uint64_t decision_id)
{
    if (coordinator_id >= g_coordinator_count)
    {
        return -1;
    }

    IntelligenceCoordinator *coord = &g_coordinators[coordinator_id];
    if (decision_id >= coord->decision_count)
    {
        return -1;
    }

    AIDecision *decision = &coord->ai_decisions[decision_id];
    decision->current_state = DECISION_STATE_FAILED;

    return 0;
}

int ic_execute_decision(uint32_t coordinator_id, uint64_t decision_id)
{
    if (coordinator_id >= g_coordinator_count)
    {
        return -1;
    }

    IntelligenceCoordinator *coord = &g_coordinators[coordinator_id];
    if (decision_id >= coord->decision_count)
    {
        return -1;
    }

    AIDecision *decision = &coord->ai_decisions[decision_id];
    decision->current_state = DECISION_STATE_EXECUTING;
    decision->execution_time = time(NULL);
    coord->decisions_executed++;

    return 0;
}

int ic_verify_decision_outcome(uint32_t coordinator_id, uint64_t decision_id, int32_t outcome)
{
    if (coordinator_id >= g_coordinator_count)
    {
        return -1;
    }

    IntelligenceCoordinator *coord = &g_coordinators[coordinator_id];
    if (decision_id >= coord->decision_count)
    {
        return -1;
    }

    AIDecision *decision = &coord->ai_decisions[decision_id];
    decision->outcome_score = outcome;
    decision->verification_time = time(NULL);

    if (outcome > 0)
    {
        decision->current_state = DECISION_STATE_VERIFIED;
        coord->decisions_successful++;
        if (decision->origin_subsystem < MAX_SUBSYSTEM_AGENTS)
        {
            coord->subsystem_agents[decision->origin_subsystem].decisions_successful++;
        }
    }
    else
    {
        decision->current_state = DECISION_STATE_FAILED;
    }

    return 0;
}

int ic_revert_decision(uint32_t coordinator_id, uint64_t decision_id)
{
    if (coordinator_id >= g_coordinator_count)
    {
        return -1;
    }

    IntelligenceCoordinator *coord = &g_coordinators[coordinator_id];
    if (decision_id >= coord->decision_count)
    {
        return -1;
    }

    AIDecision *decision = &coord->ai_decisions[decision_id];
    if (decision->is_reversible)
    {
        decision->current_state = DECISION_STATE_REVERTED;
        return 0;
    }

    return -1;
}

int ic_add_decision_rule(uint32_t coordinator_id, DecisionType decision_type,
                         const char *rule_name, const char *condition,
                         const char *recommended_action, uint32_t priority)
{
    if (coordinator_id >= g_coordinator_count)
    {
        return -1;
    }

    IntelligenceCoordinator *coord = &g_coordinators[coordinator_id];
    if (coord->rule_count >= MAX_DECISION_RULES)
    {
        return -1;
    }

    DecisionRule *rule = &coord->decision_rules[coord->rule_count];
    rule->rule_id = coord->rule_count;
    rule->decision_type = decision_type;
    strncpy(rule->rule_name, rule_name, 63);
    rule->rule_name[63] = '\0';
    strncpy(rule->condition_expression, condition, 255);
    rule->condition_expression[255] = '\0';
    strncpy(rule->recommended_action, recommended_action, 255);
    rule->recommended_action[255] = '\0';
    rule->priority = priority;
    rule->applications = 0;
    rule->effectiveness_rate = 0.0;
    rule->is_enabled = 1;

    coord->rule_count++;

    return rule->rule_id;
}

int ic_remove_decision_rule(uint32_t coordinator_id, uint32_t rule_id)
{
    if (coordinator_id >= g_coordinator_count)
    {
        return -1;
    }

    IntelligenceCoordinator *coord = &g_coordinators[coordinator_id];
    if (rule_id >= coord->rule_count)
    {
        return -1;
    }

    coord->decision_rules[rule_id].is_enabled = 0;

    return 0;
}

int ic_enable_decision_rule(uint32_t coordinator_id, uint32_t rule_id)
{
    if (coordinator_id >= g_coordinator_count)
    {
        return -1;
    }

    IntelligenceCoordinator *coord = &g_coordinators[coordinator_id];
    if (rule_id >= coord->rule_count)
    {
        return -1;
    }

    coord->decision_rules[rule_id].is_enabled = 1;

    return 0;
}

int ic_disable_decision_rule(uint32_t coordinator_id, uint32_t rule_id)
{
    if (coordinator_id >= g_coordinator_count)
    {
        return -1;
    }

    IntelligenceCoordinator *coord = &g_coordinators[coordinator_id];
    if (rule_id >= coord->rule_count)
    {
        return -1;
    }

    coord->decision_rules[rule_id].is_enabled = 0;

    return 0;
}

int ic_detect_decision_conflict(uint32_t coordinator_id, const AIDecision *decision1,
                                const AIDecision *decision2)
{
    if (coordinator_id >= g_coordinator_count || !decision1 || !decision2)
    {
        return -1;
    }

    IntelligenceCoordinator *coord = &g_coordinators[coordinator_id];

    if (decision1->decision_type != decision2->decision_type)
    {
        return 0;
    }

    if (decision1->priority != decision2->priority)
    {
        return 0;
    }

    coord->conflicts_detected++;

    return 1;
}

int ic_resolve_conflict(uint32_t coordinator_id, uint64_t decision_id1,
                        uint64_t decision_id2, ConflictResolutionStrategy strategy)
{
    if (coordinator_id >= g_coordinator_count)
    {
        return -1;
    }

    IntelligenceCoordinator *coord = &g_coordinators[coordinator_id];
    if (decision_id1 >= coord->decision_count || decision_id2 >= coord->decision_count)
    {
        return -1;
    }

    AIDecision *d1 = &coord->ai_decisions[decision_id1];
    AIDecision *d2 = &coord->ai_decisions[decision_id2];

    AIDecision *winner = NULL;

    switch (strategy)
    {
    case CONFLICT_RESOLUTION_PRIORITY:
        winner = (d1->priority > d2->priority) ? d1 : d2;
        break;
    case CONFLICT_RESOLUTION_WEIGHTED_SCORE:
        winner = (d1->confidence_score > d2->confidence_score) ? d1 : d2;
        break;
    case CONFLICT_RESOLUTION_TIME_BASED:
        winner = (d1->creation_time < d2->creation_time) ? d1 : d2;
        break;
    case CONFLICT_RESOLUTION_IMPACT_ANALYSIS:
        winner = (d1->priority > d2->priority) ? d1 : d2;
        break;
    case CONFLICT_RESOLUTION_CONSENSUS:
        winner = d1;
        break;
    default:
        winner = d1;
        break;
    }

    if (winner == d1)
    {
        d2->current_state = DECISION_STATE_FAILED;
    }
    else
    {
        d1->current_state = DECISION_STATE_FAILED;
    }

    coord->conflicts_resolved++;

    return (winner == d1) ? 0 : 1;
}

int ic_record_learning_sample(uint32_t coordinator_id, DecisionType decision_type,
                              const char *input_features, const char *action,
                              int32_t outcome, double reward)
{
    if (coordinator_id >= g_coordinator_count)
    {
        return -1;
    }

    IntelligenceCoordinator *coord = &g_coordinators[coordinator_id];
    if (coord->sample_count >= MAX_LEARNING_SAMPLES)
    {
        return -1;
    }

    LearningSample *sample = &coord->learning_samples[coord->sample_count];
    sample->sample_id = coord->sample_count;
    sample->decision_type = decision_type;
    sample->state_hash = (uint64_t)time(NULL);
    strncpy(sample->input_features, input_features, 511);
    sample->input_features[511] = '\0';
    strncpy(sample->executed_action, action, 255);
    sample->executed_action[255] = '\0';
    sample->outcome_score = outcome;
    sample->reward = reward;
    sample->timestamp = time(NULL);

    coord->sample_count++;

    return sample->sample_id;
}

int ic_perform_learning_iteration(uint32_t coordinator_id, LearningStrategy strategy)
{
    if (coordinator_id >= g_coordinator_count)
    {
        return -1;
    }

    IntelligenceCoordinator *coord = &g_coordinators[coordinator_id];
    coord->learning_iterations++;

    uint32_t successful = 0;
    for (uint32_t i = 0; i < coord->sample_count; i++)
    {
        if (coord->learning_samples[i].reward > 0.0)
        {
            successful++;
        }
    }

    if (coord->sample_count > 0)
    {
        coord->average_outcome_score = (double)successful / (double)coord->sample_count;
    }

    return 0;
}

int ic_update_decision_rules_from_learning(uint32_t coordinator_id)
{
    if (coordinator_id >= g_coordinator_count)
    {
        return -1;
    }

    IntelligenceCoordinator *coord = &g_coordinators[coordinator_id];

    for (uint32_t i = 0; i < coord->rule_count; i++)
    {
        DecisionRule *rule = &coord->decision_rules[i];

        uint32_t applications = 0;
        uint32_t successes = 0;

        for (uint32_t j = 0; j < coord->sample_count; j++)
        {
            if (coord->learning_samples[j].decision_type == rule->decision_type)
            {
                applications++;
                if (coord->learning_samples[j].reward > 0.0)
                {
                    successes++;
                }
            }
        }

        rule->applications = applications;
        if (applications > 0)
        {
            rule->effectiveness_rate = (double)successes / (double)applications;
        }
    }

    return 0;
}

int ic_orchestrate_system_optimization(uint32_t coordinator_id)
{
    if (coordinator_id >= g_coordinator_count)
    {
        return -1;
    }

    IntelligenceCoordinator *coord = &g_coordinators[coordinator_id];
    coord->last_optimization = time(NULL);

    ic_perform_learning_iteration(coordinator_id, coord->learning_strategy);
    ic_update_decision_rules_from_learning(coordinator_id);

    if (coord->decision_count > 0)
    {
        coord->average_confidence_score = 0.0;
        for (uint32_t i = 0; i < coord->decision_count; i++)
        {
            coord->average_confidence_score += coord->ai_decisions[i].confidence_score;
        }
        coord->average_confidence_score /= (double)coord->decision_count;
    }

    return 0;
}

int ic_get_optimal_decision_path(uint32_t coordinator_id, DecisionType decision_type,
                                 uint64_t *decision_ids, uint32_t *count)
{
    if (coordinator_id >= g_coordinator_count || !decision_ids || !count)
    {
        return -1;
    }

    IntelligenceCoordinator *coord = &g_coordinators[coordinator_id];
    uint32_t idx = 0;

    for (uint32_t i = 0; i < coord->decision_count && idx < *count; i++)
    {
        if (coord->ai_decisions[i].decision_type == decision_type &&
            coord->ai_decisions[i].current_state == DECISION_STATE_VERIFIED)
        {
            decision_ids[idx++] = coord->ai_decisions[i].decision_id;
        }
    }

    *count = idx;

    return 0;
}

double ic_get_system_intelligence_score(uint32_t coordinator_id)
{
    if (coordinator_id >= g_coordinator_count)
    {
        return 0.0;
    }

    IntelligenceCoordinator *coord = &g_coordinators[coordinator_id];

    if (coord->total_decisions_made == 0)
    {
        return 0.0;
    }

    double success_rate = (double)coord->decisions_successful / (double)coord->total_decisions_made;
    double confidence = coord->average_confidence_score / 100.0;
    double outcome = (coord->average_outcome_score + 1.0) / 2.0;

    double intelligence_score = (success_rate * 0.4) + (confidence * 0.3) + (outcome * 0.3);

    return intelligence_score * 100.0;
}

int ic_get_subsystem_agent_recommendations(uint32_t coordinator_id,
                                           SubsystemType subsystem_type,
                                           AIDecision *recommendations, uint32_t *count)
{
    if (coordinator_id >= g_coordinator_count || !recommendations || !count)
    {
        return -1;
    }

    IntelligenceCoordinator *coord = &g_coordinators[coordinator_id];
    uint32_t idx = 0;

    for (uint32_t i = 0; i < coord->decision_count && idx < *count; i++)
    {
        if (coord->ai_decisions[i].origin_subsystem == subsystem_type &&
            coord->ai_decisions[i].current_state == DECISION_STATE_PROPOSED)
        {
            recommendations[idx++] = coord->ai_decisions[i];
        }
    }

    *count = idx;

    return 0;
}

IntelligenceCoordinator *ic_get_coordinator_info(uint32_t coordinator_id)
{
    if (coordinator_id >= g_coordinator_count)
    {
        return NULL;
    }

    return &g_coordinators[coordinator_id];
}
