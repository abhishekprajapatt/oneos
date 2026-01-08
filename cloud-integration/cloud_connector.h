#ifndef CLOUD_CONNECTOR_H
#define CLOUD_CONNECTOR_H

#include <stdint.h>
#include <stddef.h>

#define MAX_CLOUD_PROVIDERS 16
#define MAX_CLOUD_CONNECTIONS 256
#define MAX_CLOUD_CREDENTIALS 512
#define MAX_CLOUD_REGIONS 64

typedef enum
{
    CLOUD_PROVIDER_AWS,
    CLOUD_PROVIDER_AZURE,
    CLOUD_PROVIDER_GCP,
    CLOUD_PROVIDER_ALIBABA,
    CLOUD_PROVIDER_IBMCLOUD,
    CLOUD_PROVIDER_ORACLE
} CloudProvider;

typedef enum
{
    CREDENTIAL_TYPE_API_KEY,
    CREDENTIAL_TYPE_ACCESS_KEY_SECRET,
    CREDENTIAL_TYPE_CERTIFICATE,
    CREDENTIAL_TYPE_SERVICE_PRINCIPAL,
    CREDENTIAL_TYPE_OAUTH2_TOKEN
} CredentialType;

typedef enum
{
    CONNECTION_STATUS_DISCONNECTED,
    CONNECTION_STATUS_CONNECTING,
    CONNECTION_STATUS_CONNECTED,
    CONNECTION_STATUS_AUTHENTICATING,
    CONNECTION_STATUS_ERROR
} ConnectionStatus;

typedef struct
{
    uint32_t credential_id;
    CredentialType credential_type;

    char credential_key[512];
    char credential_secret[512];
    char credential_certificate[1024];

    uint64_t credential_created_time;
    uint64_t credential_expires_time;
    uint8_t is_valid;
} CloudCredential;

typedef struct
{
    uint32_t connection_id;
    CloudProvider provider;

    uint32_t credential_id;
    char region_name[64];

    ConnectionStatus status;

    uint64_t connection_start_time;
    uint64_t last_heartbeat_time;
    uint32_t connection_timeout_seconds;

    uint32_t failed_connection_attempts;
    uint8_t is_authenticated;
    uint8_t is_encrypted;
} CloudConnection;

typedef struct
{
    uint32_t connector_id;

    CloudProvider providers[MAX_CLOUD_PROVIDERS];
    uint32_t provider_count;

    CloudCredential credentials[MAX_CLOUD_CREDENTIALS];
    uint32_t credential_count;

    CloudConnection connections[MAX_CLOUD_CONNECTIONS];
    uint32_t connection_count;

    char active_region[64];
    uint32_t active_provider;

    uint64_t total_api_calls;
    uint32_t successful_connections;
    uint32_t failed_connections;
} CloudConnector;

CloudConnector *cloud_connector_init(void);
int cloud_connector_destroy(CloudConnector *connector);

int cloud_connector_register_provider(CloudConnector *connector, CloudProvider provider);

uint32_t cloud_credential_create(CloudConnector *connector, CredentialType type,
                                 const char *key, const char *secret);

int cloud_credential_validate(CloudConnector *connector, uint32_t credential_id);

int cloud_credential_revoke(CloudConnector *connector, uint32_t credential_id);

uint32_t cloud_connection_create(CloudConnector *connector, CloudProvider provider,
                                 uint32_t credential_id, const char *region);

int cloud_connection_authenticate(CloudConnector *connector, uint32_t connection_id);

int cloud_connection_connect(CloudConnector *connector, uint32_t connection_id);

int cloud_connection_disconnect(CloudConnector *connector, uint32_t connection_id);

int cloud_connection_check_status(CloudConnector *connector, uint32_t connection_id,
                                  ConnectionStatus *status);

int cloud_connection_send_heartbeat(CloudConnector *connector, uint32_t connection_id);

int cloud_connector_set_active_connection(CloudConnector *connector, uint32_t connection_id);

int cloud_connector_switch_region(CloudConnector *connector, const char *region);

int cloud_connector_list_regions(CloudConnector *connector, CloudProvider provider,
                                 char *regions[], uint32_t *region_count);

int cloud_connector_execute_api_call(CloudConnector *connector, uint32_t connection_id,
                                     const char *service, const char *operation);

typedef struct
{
    uint32_t connector_id;
    uint32_t provider_count;
    uint32_t credential_count;
    uint32_t connection_count;
    uint32_t active_connections;
    uint32_t successful_connections;
    uint32_t failed_connections;
    uint64_t total_api_calls;
} CloudConnectorMetrics;

int cloud_connector_get_metrics(CloudConnector *connector, CloudConnectorMetrics *metrics);

#endif
