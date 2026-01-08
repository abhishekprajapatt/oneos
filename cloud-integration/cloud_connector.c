#include "cloud_connector.h"
#include <stdlib.h>
#include <string.h>

static CloudConnector *global_cloud_connector = NULL;
static uint32_t next_credential_id = 1;
static uint32_t next_connection_id = 1;

CloudConnector *cloud_connector_init(void)
{
    CloudConnector *connector = (CloudConnector *)malloc(sizeof(CloudConnector));
    if (!connector)
        return NULL;

    memset(connector, 0, sizeof(CloudConnector));
    connector->connector_id = 1;
    connector->provider_count = 0;
    connector->credential_count = 0;
    connector->connection_count = 0;
    connector->active_provider = 0;
    connector->total_api_calls = 0;
    connector->successful_connections = 0;
    connector->failed_connections = 0;

    global_cloud_connector = connector;
    return connector;
}

int cloud_connector_destroy(CloudConnector *connector)
{
    if (!connector)
        return -1;

    free(connector);
    global_cloud_connector = NULL;
    return 0;
}

int cloud_connector_register_provider(CloudConnector *connector, CloudProvider provider)
{
    if (!connector || connector->provider_count >= MAX_CLOUD_PROVIDERS)
        return -1;

    for (uint32_t i = 0; i < connector->provider_count; i++)
    {
        if (connector->providers[i] == provider)
            return -1;
    }

    connector->providers[connector->provider_count++] = provider;
    return 0;
}

uint32_t cloud_credential_create(CloudConnector *connector, CredentialType type,
                                 const char *key, const char *secret)
{
    if (!connector || connector->credential_count >= MAX_CLOUD_CREDENTIALS || !key)
        return 0;

    CloudCredential *credential = &connector->credentials[connector->credential_count];
    memset(credential, 0, sizeof(CloudCredential));

    credential->credential_id = next_credential_id++;
    credential->credential_type = type;

    strncpy(credential->credential_key, key, sizeof(credential->credential_key) - 1);

    if (secret)
    {
        strncpy(credential->credential_secret, secret, sizeof(credential->credential_secret) - 1);
    }

    credential->credential_created_time = 0;
    credential->credential_expires_time = 0;
    credential->is_valid = 1;

    connector->credential_count++;
    return credential->credential_id;
}

int cloud_credential_validate(CloudConnector *connector, uint32_t credential_id)
{
    if (!connector || credential_id == 0)
        return -1;

    for (uint32_t i = 0; i < connector->credential_count; i++)
    {
        if (connector->credentials[i].credential_id == credential_id)
        {
            connector->credentials[i].is_valid = 1;
            return 0;
        }
    }

    return -1;
}

int cloud_credential_revoke(CloudConnector *connector, uint32_t credential_id)
{
    if (!connector || credential_id == 0)
        return -1;

    for (uint32_t i = 0; i < connector->credential_count; i++)
    {
        if (connector->credentials[i].credential_id == credential_id)
        {
            connector->credentials[i].is_valid = 0;
            return 0;
        }
    }

    return -1;
}

uint32_t cloud_connection_create(CloudConnector *connector, CloudProvider provider,
                                 uint32_t credential_id, const char *region)
{
    if (!connector || connector->connection_count >= MAX_CLOUD_CONNECTIONS || !region)
        return 0;

    CloudConnection *connection = &connector->connections[connector->connection_count];
    memset(connection, 0, sizeof(CloudConnection));

    connection->connection_id = next_connection_id++;
    connection->provider = provider;
    connection->credential_id = credential_id;

    strncpy(connection->region_name, region, sizeof(connection->region_name) - 1);

    connection->status = CONNECTION_STATUS_DISCONNECTED;
    connection->connection_start_time = 0;
    connection->last_heartbeat_time = 0;
    connection->connection_timeout_seconds = 30;
    connection->failed_connection_attempts = 0;
    connection->is_authenticated = 0;
    connection->is_encrypted = 1;

    connector->connection_count++;
    return connection->connection_id;
}

int cloud_connection_authenticate(CloudConnector *connector, uint32_t connection_id)
{
    if (!connector || connection_id == 0)
        return -1;

    for (uint32_t i = 0; i < connector->connection_count; i++)
    {
        if (connector->connections[i].connection_id == connection_id)
        {
            CloudConnection *connection = &connector->connections[i];

            if (connection->credential_id == 0)
                return -1;

            connection->status = CONNECTION_STATUS_AUTHENTICATING;
            connection->is_authenticated = 1;
            connection->failed_connection_attempts = 0;

            return 0;
        }
    }

    return -1;
}

int cloud_connection_connect(CloudConnector *connector, uint32_t connection_id)
{
    if (!connector || connection_id == 0)
        return -1;

    for (uint32_t i = 0; i < connector->connection_count; i++)
    {
        if (connector->connections[i].connection_id == connection_id)
        {
            CloudConnection *connection = &connector->connections[i];

            if (!connection->is_authenticated)
            {
                connection->status = CONNECTION_STATUS_ERROR;
                connection->failed_connection_attempts++;
                connector->failed_connections++;
                return -1;
            }

            connection->status = CONNECTION_STATUS_CONNECTED;
            connection->connection_start_time = 0;
            connection->failed_connection_attempts = 0;
            connector->successful_connections++;

            return 0;
        }
    }

    return -1;
}

int cloud_connection_disconnect(CloudConnector *connector, uint32_t connection_id)
{
    if (!connector || connection_id == 0)
        return -1;

    for (uint32_t i = 0; i < connector->connection_count; i++)
    {
        if (connector->connections[i].connection_id == connection_id)
        {
            CloudConnection *connection = &connector->connections[i];
            connection->status = CONNECTION_STATUS_DISCONNECTED;
            connection->is_authenticated = 0;
            return 0;
        }
    }

    return -1;
}

int cloud_connection_check_status(CloudConnector *connector, uint32_t connection_id,
                                  ConnectionStatus *status)
{
    if (!connector || connection_id == 0 || !status)
        return -1;

    for (uint32_t i = 0; i < connector->connection_count; i++)
    {
        if (connector->connections[i].connection_id == connection_id)
        {
            *status = connector->connections[i].status;
            return 0;
        }
    }

    return -1;
}

int cloud_connection_send_heartbeat(CloudConnector *connector, uint32_t connection_id)
{
    if (!connector || connection_id == 0)
        return -1;

    for (uint32_t i = 0; i < connector->connection_count; i++)
    {
        if (connector->connections[i].connection_id == connection_id)
        {
            CloudConnection *connection = &connector->connections[i];

            if (connection->status == CONNECTION_STATUS_CONNECTED)
            {
                connection->last_heartbeat_time = 0;
                connector->total_api_calls++;
                return 0;
            }
        }
    }

    return -1;
}

int cloud_connector_set_active_connection(CloudConnector *connector, uint32_t connection_id)
{
    if (!connector || connection_id == 0)
        return -1;

    for (uint32_t i = 0; i < connector->connection_count; i++)
    {
        if (connector->connections[i].connection_id == connection_id)
        {
            connector->active_provider = connector->connections[i].provider;
            return 0;
        }
    }

    return -1;
}

int cloud_connector_switch_region(CloudConnector *connector, const char *region)
{
    if (!connector || !region)
        return -1;

    strncpy(connector->active_region, region, sizeof(connector->active_region) - 1);
    return 0;
}

int cloud_connector_list_regions(CloudConnector *connector, CloudProvider provider,
                                 char *regions[], uint32_t *region_count)
{
    if (!connector || !regions || !region_count)
        return -1;

    uint32_t count = 0;

    for (uint32_t i = 0; i < connector->connection_count; i++)
    {
        if (connector->connections[i].provider == provider && count < 32)
        {
            regions[count++] = connector->connections[i].region_name;
        }
    }

    *region_count = count;
    return 0;
}

int cloud_connector_execute_api_call(CloudConnector *connector, uint32_t connection_id,
                                     const char *service, const char *operation)
{
    if (!connector || connection_id == 0 || !service || !operation)
        return -1;

    for (uint32_t i = 0; i < connector->connection_count; i++)
    {
        if (connector->connections[i].connection_id == connection_id)
        {
            CloudConnection *connection = &connector->connections[i];

            if (connection->status == CONNECTION_STATUS_CONNECTED)
            {
                connector->total_api_calls++;
                return 0;
            }
        }
    }

    return -1;
}

int cloud_connector_get_metrics(CloudConnector *connector, CloudConnectorMetrics *metrics)
{
    if (!connector || !metrics)
        return -1;

    metrics->connector_id = connector->connector_id;
    metrics->provider_count = connector->provider_count;
    metrics->credential_count = connector->credential_count;
    metrics->connection_count = connector->connection_count;

    uint32_t active = 0;
    for (uint32_t i = 0; i < connector->connection_count; i++)
    {
        if (connector->connections[i].status == CONNECTION_STATUS_CONNECTED)
        {
            active++;
        }
    }

    metrics->active_connections = active;
    metrics->successful_connections = connector->successful_connections;
    metrics->failed_connections = connector->failed_connections;
    metrics->total_api_calls = connector->total_api_calls;

    return 0;
}
