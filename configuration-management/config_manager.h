#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <stdint.h>
#include <stddef.h>

#define MAX_CONFIGS 64
#define MAX_CONFIG_KEYS_PER_CONFIG 128
#define MAX_CONFIG_WATCHERS 32
#define MAX_CONFIG_VERSIONS_PER_CONFIG 16

typedef enum
{
    CONFIG_TYPE_STRING,
    CONFIG_TYPE_INTEGER,
    CONFIG_TYPE_BOOLEAN
} ConfigType;

typedef enum
{
    CONFIG_SOURCE_FILE,
    CONFIG_SOURCE_ENVIRONMENT,
    CONFIG_SOURCE_RUNTIME
} ConfigSource;

typedef struct
{
    uint32_t key_id;
    char key_name[128];
    ConfigType value_type;
    char value_data[512];
    ConfigSource source;
    uint8_t is_encrypted;
} ConfigKey;

typedef struct
{
    uint32_t config_id;
    char config_name[256];

    ConfigKey keys[MAX_CONFIG_KEYS_PER_CONFIG];
    uint32_t key_count;

    uint64_t total_updates;
    uint8_t auto_reload_enabled;
} Config;

typedef struct
{
    uint32_t manager_id;

    Config configs[MAX_CONFIGS];
    uint32_t config_count;

    uint64_t total_config_reads;
    uint64_t total_config_writes;
} ConfigManager;

ConfigManager *config_manager_init(void);
int config_manager_destroy(ConfigManager *manager);

uint32_t config_create(ConfigManager *manager, const char *config_name);
int config_destroy(ConfigManager *manager, uint32_t config_id);

int config_set_key(ConfigManager *manager, uint32_t config_id, const char *key_name,
                   ConfigType type, const char *value, ConfigSource source);

int config_get_key(ConfigManager *manager, uint32_t config_id, const char *key_name,
                   char *value_buffer, uint32_t buffer_size);

int config_delete_key(ConfigManager *manager, uint32_t config_id, const char *key_name);

int config_key_encrypt(ConfigManager *manager, uint32_t config_id, const char *key_name);

int config_enable_auto_reload(ConfigManager *manager, uint32_t config_id,
                              uint32_t interval_seconds);

int config_disable_auto_reload(ConfigManager *manager, uint32_t config_id);

int config_reload(ConfigManager *manager, uint32_t config_id);

int config_validate_keys(ConfigManager *manager, uint32_t config_id);

int config_export_to_file(ConfigManager *manager, uint32_t config_id, const char *filepath);

int config_import_from_file(ConfigManager *manager, uint32_t config_id, const char *filepath);

typedef struct
{
    uint32_t manager_id;
    uint32_t total_configs;
    uint32_t active_configs;
    uint64_t total_reads;
    uint64_t total_writes;
} ConfigManagerMetrics;

int config_manager_get_metrics(ConfigManager *manager, ConfigManagerMetrics *metrics);

#endif
