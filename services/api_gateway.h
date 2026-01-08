#ifndef API_GATEWAY_H
#define API_GATEWAY_H

#include <stdint.h>
#include <stddef.h>

#define MAX_API_GATEWAYS 8
#define MAX_ROUTES 512
#define MAX_BACKENDS 256
#define MAX_API_KEYS 1024
#define MAX_RATE_LIMITS 128
#define MAX_PLUGINS 64
#define MAX_ACTIVE_REQUESTS 4096

typedef enum
{
    ROUTE_HTTP_GET = 1,
    ROUTE_HTTP_POST = 2,
    ROUTE_HTTP_PUT = 3,
    ROUTE_HTTP_DELETE = 4,
    ROUTE_HTTP_PATCH = 5,
    ROUTE_WEBSOCKET = 6,
    ROUTE_GRPC = 7
} RouteType;

typedef enum
{
    BACKEND_HTTP = 1,
    BACKEND_GRPC = 2,
    BACKEND_KAFKA = 3,
    BACKEND_DATABASE = 4,
    BACKEND_CACHE = 5
} BackendType;

typedef enum
{
    AUTH_NONE = 0,
    AUTH_API_KEY = 1,
    AUTH_JWT = 2,
    AUTH_OAUTH2 = 3,
    AUTH_MUTUAL_TLS = 4
} AuthType;

typedef enum
{
    POLICY_RATE_LIMIT = 1,
    POLICY_TIMEOUT = 2,
    POLICY_CIRCUIT_BREAK = 3,
    POLICY_RETRY = 4,
    POLICY_CORS = 5
} PolicyType;

typedef struct
{
    uint32_t backend_id;
    char name[64];
    BackendType backend_type;
    char address[256];
    uint16_t port;
    uint32_t healthy;
    uint64_t request_count;
    uint64_t error_count;
    float avg_latency_ms;
} Backend;

typedef struct
{
    uint32_t route_id;
    char path[256];
    RouteType route_type;
    char method[16];

    uint32_t backend_ids[16];
    uint32_t backend_count;

    AuthType auth_type;
    uint32_t rate_limit_requests;
    uint32_t rate_limit_window_sec;

    uint32_t timeout_ms;
    uint32_t retry_count;
    uint32_t is_enabled;
} APIRoute;

typedef struct
{
    uint64_t api_key_id;
    char api_key[128];
    char client_name[64];
    uint32_t is_active;
    uint64_t created_at;
    uint64_t expires_at;
    uint32_t rate_limit_rps;
} APIKey;

typedef struct
{
    uint64_t request_id;
    uint32_t route_id;
    uint32_t backend_id;

    char client_ip[64];
    char request_path[256];
    char request_method[16];

    uint64_t request_time;
    uint64_t response_time;
    uint32_t status_code;
    uint64_t request_bytes;
    uint64_t response_bytes;
} GatewayRequest;

typedef struct
{
    uint32_t gateway_id;
    char name[64];

    Backend backends[MAX_BACKENDS];
    uint32_t backend_count;

    APIRoute routes[MAX_ROUTES];
    uint32_t route_count;

    APIKey api_keys[MAX_API_KEYS];
    uint32_t api_key_count;

    GatewayRequest requests[MAX_ACTIVE_REQUESTS];
    uint32_t request_count;

    uint32_t enable_authentication;
    uint32_t enable_rate_limiting;
    uint32_t enable_logging;
    uint32_t enable_metrics;

    uint64_t total_requests;
    uint64_t total_errors;
    uint64_t total_bytes_in;
    uint64_t total_bytes_out;
} APIGateway;

int gw_create_gateway(const char *name);
int gw_delete_gateway(uint32_t gateway_id);

int gw_add_backend(uint32_t gateway_id, const char *name, BackendType backend_type,
                   const char *address, uint16_t port);
int gw_remove_backend(uint32_t gateway_id, uint32_t backend_id);
int gw_update_backend_health(uint32_t gateway_id, uint32_t backend_id, uint32_t healthy);
int gw_get_backend_stats(uint32_t gateway_id, uint32_t backend_id,
                         uint64_t *request_count, float *avg_latency);

int gw_add_route(uint32_t gateway_id, const char *path, RouteType route_type,
                 const char *method, AuthType auth_type);
int gw_remove_route(uint32_t gateway_id, uint32_t route_id);
int gw_add_backend_to_route(uint32_t gateway_id, uint32_t route_id, uint32_t backend_id);
int gw_remove_backend_from_route(uint32_t gateway_id, uint32_t route_id, uint32_t backend_id);
int gw_set_route_rate_limit(uint32_t gateway_id, uint32_t route_id, uint32_t requests, uint32_t window_sec);
int gw_set_route_timeout(uint32_t gateway_id, uint32_t route_id, uint32_t timeout_ms);

int gw_issue_api_key(uint32_t gateway_id, const char *client_name, uint32_t rate_limit_rps);
int gw_revoke_api_key(uint32_t gateway_id, uint64_t api_key_id);
int gw_validate_api_key(uint32_t gateway_id, const char *api_key, uint32_t *is_valid);

int gw_handle_request(uint32_t gateway_id, const char *path, const char *method,
                      const char *client_ip, uint32_t *route_id, uint32_t *backend_id);
int gw_log_request(uint32_t gateway_id, uint64_t request_id, uint32_t status_code,
                   uint64_t latency_ms, uint64_t request_bytes, uint64_t response_bytes);

int gw_enable_authentication(uint32_t gateway_id, uint32_t enable);
int gw_enable_rate_limiting(uint32_t gateway_id, uint32_t enable);
int gw_enable_logging(uint32_t gateway_id, uint32_t enable);
int gw_enable_metrics(uint32_t gateway_id, uint32_t enable);

uint64_t gw_get_total_requests(uint32_t gateway_id);
uint64_t gw_get_total_errors(uint32_t gateway_id);
uint64_t gw_get_bytes_in(uint32_t gateway_id);
uint64_t gw_get_bytes_out(uint32_t gateway_id);

int gw_export_metrics(uint32_t gateway_id, const char *format);
APIGateway *gw_get_gateway_info(uint32_t gateway_id);

#endif
