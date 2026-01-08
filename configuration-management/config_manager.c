#include "config_manager.h"
#include <stdlib.h>
#include <string.h>

static ConfigManager *global_config_manager = NULL;
static uint32_t next_config_id = 1;

ConfigManager *config_manager_init(void)
{
    ConfigManager *manager = (ConfigManager *)malloc(sizeof(ConfigManager));
    if (!manager)
        return NULL;

    memset(manager, 0, sizeof(ConfigManager));
    manager->manager_id = 1;
    manager->config_count = 0;
    manager->total_config_reads = 0;
    manager->total_config_writes = 0;

    global_config_manager = manager;
    return manager;
}

int config_manager_destroy(ConfigManager *manager)
{
    if (!manager)
        return -1;

    free(manager);
    global_config_manager = NULL;
    return 0;
}

uint32_t config_create(ConfigManager *manager, const char *config_name)
{
    if (!manager || manager->config_count >= MAX_CONFIGS || !config_name)
        return 0;

    Config *config = &manager->configs[manager->config_count];
    memset(config, 0, sizeof(Config));

    config->config_id = next_config_id++;
    strncpy(config->config_name, config_name, sizeof(config->config_name) - 1);

    config->key_count = 0;
    config->total_updates = 0;
    config->auto_reload_enabled = 0;

    manager->config_count++;

    return config->config_id;
}

int config_destroy(ConfigManager *manager, uint32_t config_id)
{
    if (!manager || config_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->config_count; i++)
    {
        if (manager->configs[i].config_id == config_id)
        {
            memmove(&manager->configs[i], &manager->configs[i + 1],
                    sizeof(Config) * (manager->config_count - i - 1));
            manager->config_count--;
            return 0;
        }
    }

    return -1;
}

int config_set_key(ConfigManager *manager, uint32_t config_id, const char *key_name,
                   ConfigType type, const char *value, ConfigSource source)
{
    if (!manager || config_id == 0 || !key_name || !value)
        return -1;

    for (uint32_t i = 0; i < manager->config_count; i++)
    {
        if (manager->configs[i].config_id == config_id)
        {
            Config *config = &manager->configs[i];

            if (config->key_count >= MAX_CONFIG_KEYS_PER_CONFIG)
                return -1;

            ConfigKey *key = &config->keys[config->key_count];
            memset(key, 0, sizeof(ConfigKey));

            key->key_id = config->key_count + 1;
            strncpy(key->key_name, key_name, sizeof(key->key_name) - 1);
            key->value_type = type;
            strncpy(key->value_data, value, sizeof(key->value_data) - 1);
            key->source = source;
            key->is_encrypted = 0;

            config->key_count++;
            config->total_updates++;
            manager->total_config_writes++;

            return 0;
        }
    }

    return -1;
}

int config_get_key(ConfigManager *manager, uint32_t config_id, const char *key_name,
                   char *value_buffer, uint32_t buffer_size)
{
    if (!manager || config_id == 0 || !key_name || !value_buffer || buffer_size == 0)
        return -1;

    for (uint32_t i = 0; i < manager->config_count; i++)
    {
        if (manager->configs[i].config_id == config_id)
        {
            Config *config = &manager->configs[i];

            for (uint32_t j = 0; j < config->key_count; j++)
            {
                if (strncmp(config->keys[j].key_name, key_name, sizeof(config->keys[j].key_name)) == 0)
                {
                    strncpy(value_buffer, config->keys[j].value_data, buffer_size - 1);
                    manager->total_config_reads++;
                    return 0;
                }
            }

            return -1;
        }
    }

    return -1;
}

int config_delete_key(ConfigManager *manager, uint32_t config_id, const char *key_name)
{
    if (!manager || config_id == 0 || !key_name)
        return -1;

    for (uint32_t i = 0; i < manager->config_count; i++)
    {
        if (manager->configs[i].config_id == config_id)
        {
            Config *config = &manager->configs[i];

            for (uint32_t j = 0; j < config->key_count; j++)
            {
                if (strncmp(config->keys[j].key_name, key_name, sizeof(config->keys[j].key_name)) == 0)
                {
                    memmove(&config->keys[j], &config->keys[j + 1],
                            sizeof(ConfigKey) * (config->key_count - j - 1));
                    config->key_count--;
                    config->total_updates++;
                    manager->total_config_writes++;
                    return 0;
                }
            }

            return -1;
        }
    }

    return -1;
}

int config_key_encrypt(ConfigManager *manager, uint32_t config_id, const char *key_name)
{
    if (!manager || config_id == 0 || !key_name)
        return -1;

    for (uint32_t i = 0; i < manager->config_count; i++)
    {
        if (manager->configs[i].config_id == config_id)
        {
            Config *config = &manager->configs[i];

            for (uint32_t j = 0; j < config->key_count; j++)
            {
                if (strncmp(config->keys[j].key_name, key_name, sizeof(config->keys[j].key_name)) == 0)
                {
                    config->keys[j].is_encrypted = 1;
                    return 0;
                }
            }
        }
    }

    return -1;
}

int config_enable_auto_reload(ConfigManager *manager, uint32_t config_id,
                              uint32_t interval_seconds)
{
    if (!manager || config_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->config_count; i++)
    {
        if (manager->configs[i].config_id == config_id)
        {
            manager->configs[i].auto_reload_enabled = 1;
            return 0;
        }
    }

    return -1;
}

int config_disable_auto_reload(ConfigManager *manager, uint32_t config_id)
{
    if (!manager || config_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->config_count; i++)
    {
        if (manager->configs[i].config_id == config_id)
        {
            manager->configs[i].auto_reload_enabled = 0;
            return 0;
        }
    }

    return -1;
}

int config_reload(ConfigManager *manager, uint32_t config_id)
{
    if (!manager || config_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->config_count; i++)
    {
        if (manager->configs[i].config_id == config_id)
        {
            manager->configs[i].total_updates++;
            return 0;
        }
    }

    return -1;
}

int config_validate_keys(ConfigManager *manager, uint32_t config_id)
{
    if (!manager || config_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->config_count; i++)
    {
        if (manager->configs[i].config_id == config_id)
        {
            Config *config = &manager->configs[i];

            for (uint32_t j = 0; j < config->key_count; j++)
            {
                if (config->keys[j].key_name[0] == '\0')
                    return -1;
            }

            return 0;
        }
    }

    return -1;
}

int config_export_to_file(ConfigManager *manager, uint32_t config_id, const char *filepath)
{
    if (!manager || config_id == 0 || !filepath)
        return -1;

    for (uint32_t i = 0; i < manager->config_count; i++)
    {
        if (manager->configs[i].config_id == config_id)
        {
            return 0;
        }
    }

    return -1;
}

int config_import_from_file(ConfigManager *manager, uint32_t config_id, const char *filepath)
{
    if (!manager || config_id == 0 || !filepath)
        return -1;

    for (uint32_t i = 0; i < manager->config_count; i++)
    {
        if (manager->configs[i].config_id == config_id)
        {
            return 0;
        }
    }

    return -1;
}

int config_manager_get_metrics(ConfigManager *manager, ConfigManagerMetrics *metrics)
{
    if (!manager || !metrics)
        return -1;

    metrics->manager_id = manager->manager_id;
    metrics->total_configs = manager->config_count;
    metrics->active_configs = manager->config_count;
    metrics->total_reads = manager->total_config_reads;
    metrics->total_writes = manager->total_config_writes;

    return 0;
}
