#include "power_manager.h"
#include <stdlib.h>
#include <string.h>

PowerManager *power_init(void)
{
    PowerManager *manager = (PowerManager *)malloc(sizeof(PowerManager));
    if (!manager)
        return NULL;

    memset(manager, 0, sizeof(PowerManager));
    manager->current_state = POWER_STATE_S0;
    manager->previous_state = POWER_STATE_S5;
    manager->state_entry_time = time(NULL);
    manager->idle_state_enabled = true;
    manager->idle_timeout_ms = 60000;
    manager->sleep_timeout_ms = 300000;
    manager->cpu_count = 16;
    manager->battery_percent = 100;
    manager->battery_time_remaining_min = 480;
    manager->energy_consumed_mj = 0;
    manager->total_power_transitions = 0;
    manager->suspend_to_disk_enabled = true;
    manager->wake_on_lan_enabled = false;

    manager->thermal.state = THERMAL_STATE_NORMAL;
    manager->thermal.temp_warning_threshold = 80;
    manager->thermal.temp_critical_threshold = 95;
    manager->thermal.temp_shutdown_threshold = 105;
    manager->thermal.throttle_frequency_mhz = 1200;
    manager->thermal.is_throttling = false;
    manager->thermal.time_throttled_ms = 0;

    for (uint32_t i = 0; i < manager->cpu_count; i++)
    {
        manager->cpu_freq[i].cpu_core = i;
        manager->cpu_freq[i].current_frequency_mhz = 2400;
        manager->cpu_freq[i].max_frequency_mhz = 3600;
        manager->cpu_freq[i].min_frequency_mhz = 800;
        manager->cpu_freq[i].current_policy = CPUFREQ_POLICY_ONDEMAND;
        manager->cpu_freq[i].is_scaling_enabled = true;
        manager->cpu_freq[i].transition_latency_us = 100;
        manager->cpu_freq[i].freq_level_count = 0;
    }

    return manager;
}

void power_cleanup(PowerManager *manager)
{
    if (!manager)
        return;
    memset(manager, 0, sizeof(PowerManager));
    free(manager);
}

int power_set_state(PowerManager *manager, PowerState state)
{
    if (!manager)
        return -1;
    if (state < POWER_STATE_S0 || state > POWER_STATE_S5)
        return -1;

    manager->previous_state = manager->current_state;
    manager->current_state = state;
    manager->state_entry_time = time(NULL);
    manager->total_power_transitions++;

    switch (state)
    {
    case POWER_STATE_S0:
        break;
    case POWER_STATE_S1:
        break;
    case POWER_STATE_S3:
        for (uint32_t i = 0; i < manager->cpu_count; i++)
        {
            manager->cpu_freq[i].current_frequency_mhz =
                manager->cpu_freq[i].min_frequency_mhz;
        }
        break;
    case POWER_STATE_S4:
        if (!manager->suspend_to_disk_enabled)
            return -1;
        break;
    case POWER_STATE_S5:
        break;
    default:
        return -1;
    }

    return 0;
}

PowerState power_get_state(PowerManager *manager)
{
    if (!manager)
        return POWER_STATE_S5;
    return manager->current_state;
}

int power_transition_to_idle(PowerManager *manager)
{
    if (!manager || !manager->idle_state_enabled)
        return -1;
    return power_set_state(manager, POWER_STATE_S1);
}

int power_transition_from_idle(PowerManager *manager)
{
    if (!manager)
        return -1;
    return power_set_state(manager, POWER_STATE_S0);
}

int cpu_freq_set_policy(PowerManager *manager, uint32_t cpu_core, CPUFreqPolicy policy)
{
    if (!manager || cpu_core >= manager->cpu_count)
        return -1;
    if (policy < CPUFREQ_POLICY_PERFORMANCE || policy > CPUFREQ_POLICY_SCHEDUTIL)
        return -1;

    manager->cpu_freq[cpu_core].current_policy = policy;
    return 0;
}

CPUFreqPolicy cpu_freq_get_policy(PowerManager *manager, uint32_t cpu_core)
{
    if (!manager || cpu_core >= manager->cpu_count)
        return CPUFREQ_POLICY_ONDEMAND;
    return manager->cpu_freq[cpu_core].current_policy;
}

int cpu_freq_set_frequency(PowerManager *manager, uint32_t cpu_core, uint32_t frequency_mhz)
{
    if (!manager || cpu_core >= manager->cpu_count)
        return -1;

    CPUFreqManager *freq_mgr = &manager->cpu_freq[cpu_core];
    if (frequency_mhz < freq_mgr->min_frequency_mhz ||
        frequency_mhz > freq_mgr->max_frequency_mhz)
    {
        return -1;
    }

    freq_mgr->current_frequency_mhz = frequency_mhz;
    return 0;
}

uint32_t cpu_freq_get_frequency(PowerManager *manager, uint32_t cpu_core)
{
    if (!manager || cpu_core >= manager->cpu_count)
        return 0;
    return manager->cpu_freq[cpu_core].current_frequency_mhz;
}

int cpu_freq_add_level(PowerManager *manager, uint32_t cpu_core, CPUFreqLevel level)
{
    if (!manager || cpu_core >= manager->cpu_count)
        return -1;

    CPUFreqManager *freq_mgr = &manager->cpu_freq[cpu_core];
    if (freq_mgr->freq_level_count >= MAX_CPU_FREQ_STATES)
        return -1;

    freq_mgr->freq_levels[freq_mgr->freq_level_count] = level;
    freq_mgr->freq_level_count++;
    return 0;
}

int cpu_freq_scale_all(PowerManager *manager, CPUFreqPolicy policy)
{
    if (!manager)
        return -1;

    for (uint32_t i = 0; i < manager->cpu_count; i++)
    {
        if (cpu_freq_set_policy(manager, i, policy) != 0)
        {
            return -1;
        }
    }
    return 0;
}

int cpu_freq_enable_scaling(PowerManager *manager, uint32_t cpu_core, bool enable)
{
    if (!manager || cpu_core >= manager->cpu_count)
        return -1;

    manager->cpu_freq[cpu_core].is_scaling_enabled = enable;
    return 0;
}

int thermal_update(PowerManager *manager, uint32_t temp_celsius)
{
    if (!manager)
        return -1;

    manager->thermal.temp_celsius = temp_celsius;

    if (temp_celsius >= manager->thermal.temp_shutdown_threshold)
    {
        manager->thermal.state = THERMAL_STATE_SHUTDOWN;
        return thermal_handle_shutdown(manager);
    }
    else if (temp_celsius >= manager->thermal.temp_critical_threshold)
    {
        manager->thermal.state = THERMAL_STATE_CRITICAL;
        return thermal_handle_throttle(manager);
    }
    else if (temp_celsius >= manager->thermal.temp_warning_threshold)
    {
        manager->thermal.state = THERMAL_STATE_WARNING;
    }
    else
    {
        manager->thermal.state = THERMAL_STATE_NORMAL;
        manager->thermal.is_throttling = false;
    }

    return 0;
}

ThermalState thermal_get_state(PowerManager *manager)
{
    if (!manager)
        return THERMAL_STATE_NORMAL;
    return manager->thermal.state;
}

int thermal_handle_throttle(PowerManager *manager)
{
    if (!manager)
        return -1;

    manager->thermal.is_throttling = true;

    for (uint32_t i = 0; i < manager->cpu_count; i++)
    {
        cpu_freq_set_frequency(manager, i, manager->thermal.throttle_frequency_mhz);
    }

    return 0;
}

int thermal_handle_shutdown(PowerManager *manager)
{
    if (!manager)
        return -1;
    return power_set_state(manager, POWER_STATE_S5);
}

int power_estimate_energy(PowerManager *manager)
{
    if (!manager)
        return -1;

    uint32_t total_mw = 0;
    for (uint32_t i = 0; i < manager->cpu_count; i++)
    {
        uint32_t freq = manager->cpu_freq[i].current_frequency_mhz;
        uint32_t power_mw = (freq / 100) * 50;
        total_mw += power_mw;
    }

    manager->energy_consumed_mj += total_mw / 1000;
    return 0;
}

uint32_t power_get_battery_level(PowerManager *manager)
{
    if (!manager)
        return 0;
    return manager->battery_percent;
}

uint32_t power_get_battery_time(PowerManager *manager)
{
    if (!manager)
        return 0;
    return manager->battery_time_remaining_min;
}

int power_set_idle_timeout(PowerManager *manager, uint32_t timeout_ms)
{
    if (!manager || timeout_ms == 0)
        return -1;
    manager->idle_timeout_ms = timeout_ms;
    return 0;
}

int power_set_sleep_timeout(PowerManager *manager, uint32_t timeout_ms)
{
    if (!manager || timeout_ms == 0)
        return -1;
    manager->sleep_timeout_ms = timeout_ms;
    return 0;
}

int power_enable_wake_on_lan(PowerManager *manager, bool enable)
{
    if (!manager)
        return -1;
    manager->wake_on_lan_enabled = enable;
    return 0;
}

int power_enable_suspend_to_disk(PowerManager *manager, bool enable)
{
    if (!manager)
        return -1;
    manager->suspend_to_disk_enabled = enable;
    return 0;
}

uint64_t power_get_energy_consumed(PowerManager *manager)
{
    if (!manager)
        return 0;
    return manager->energy_consumed_mj;
}

uint64_t power_get_total_transitions(PowerManager *manager)
{
    if (!manager)
        return 0;
    return manager->total_power_transitions;
}
