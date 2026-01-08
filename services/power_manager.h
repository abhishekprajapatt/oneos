#ifndef POWER_MANAGER_H
#define POWER_MANAGER_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#define MAX_POWER_STATES 8
#define MAX_CPU_FREQ_STATES 32

typedef enum
{
    POWER_STATE_S0 = 0,
    POWER_STATE_S1 = 1,
    POWER_STATE_S3 = 3,
    POWER_STATE_S4 = 4,
    POWER_STATE_S5 = 5
} PowerState;

typedef enum
{
    CPUFREQ_POLICY_PERFORMANCE = 0,
    CPUFREQ_POLICY_POWERSAVE = 1,
    CPUFREQ_POLICY_ONDEMAND = 2,
    CPUFREQ_POLICY_CONSERVATIVE = 3,
    CPUFREQ_POLICY_SCHEDUTIL = 4
} CPUFreqPolicy;

typedef enum
{
    THERMAL_STATE_NORMAL = 0,
    THERMAL_STATE_WARNING = 1,
    THERMAL_STATE_CRITICAL = 2,
    THERMAL_STATE_SHUTDOWN = 3
} ThermalState;

typedef struct
{
    uint32_t frequency_mhz;
    uint32_t voltage_mv;
    float power_consumption_w;
    float relative_performance;
} CPUFreqLevel;

typedef struct
{
    uint32_t cpu_core;
    uint32_t current_frequency_mhz;
    uint32_t max_frequency_mhz;
    uint32_t min_frequency_mhz;
    CPUFreqLevel freq_levels[MAX_CPU_FREQ_STATES];
    uint32_t freq_level_count;
    CPUFreqPolicy current_policy;
    bool is_scaling_enabled;
    uint32_t transition_latency_us;
} CPUFreqManager;

typedef struct
{
    uint32_t temp_celsius;
    ThermalState state;
    uint32_t temp_warning_threshold;
    uint32_t temp_critical_threshold;
    uint32_t temp_shutdown_threshold;
    uint32_t throttle_frequency_mhz;
    bool is_throttling;
    uint64_t time_throttled_ms;
} ThermalInfo;

typedef struct
{
    PowerState current_state;
    PowerState previous_state;
    time_t state_entry_time;
    bool idle_state_enabled;
    uint32_t idle_timeout_ms;
    uint32_t sleep_timeout_ms;
    CPUFreqManager cpu_freq[16];
    uint32_t cpu_count;
    ThermalInfo thermal;
    uint32_t battery_percent;
    uint32_t battery_time_remaining_min;
    uint64_t energy_consumed_mj;
    uint64_t total_power_transitions;
    bool suspend_to_disk_enabled;
    bool wake_on_lan_enabled;
} PowerManager;

PowerManager *power_init(void);
void power_cleanup(PowerManager *manager);

int power_set_state(PowerManager *manager, PowerState state);
PowerState power_get_state(PowerManager *manager);
int power_transition_to_idle(PowerManager *manager);
int power_transition_from_idle(PowerManager *manager);

int cpu_freq_set_policy(PowerManager *manager, uint32_t cpu_core, CPUFreqPolicy policy);
CPUFreqPolicy cpu_freq_get_policy(PowerManager *manager, uint32_t cpu_core);
int cpu_freq_set_frequency(PowerManager *manager, uint32_t cpu_core, uint32_t frequency_mhz);
uint32_t cpu_freq_get_frequency(PowerManager *manager, uint32_t cpu_core);
int cpu_freq_add_level(PowerManager *manager, uint32_t cpu_core, CPUFreqLevel level);

int cpu_freq_scale_all(PowerManager *manager, CPUFreqPolicy policy);
int cpu_freq_enable_scaling(PowerManager *manager, uint32_t cpu_core, bool enable);

int thermal_update(PowerManager *manager, uint32_t temp_celsius);
ThermalState thermal_get_state(PowerManager *manager);
int thermal_handle_throttle(PowerManager *manager);
int thermal_handle_shutdown(PowerManager *manager);

int power_estimate_energy(PowerManager *manager);
uint32_t power_get_battery_level(PowerManager *manager);
uint32_t power_get_battery_time(PowerManager *manager);

int power_set_idle_timeout(PowerManager *manager, uint32_t timeout_ms);
int power_set_sleep_timeout(PowerManager *manager, uint32_t timeout_ms);
int power_enable_wake_on_lan(PowerManager *manager, bool enable);
int power_enable_suspend_to_disk(PowerManager *manager, bool enable);

uint64_t power_get_energy_consumed(PowerManager *manager);
uint64_t power_get_total_transitions(PowerManager *manager);

#endif
