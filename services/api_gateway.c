#include "api_gateway.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static APIGateway g_gateways[MAX_API_GATEWAYS] = {0};
static uint32_t g_gateway_count = 0;
static uint64_t g_request_id_counter = 1;
static uint64_t g_api_key_id_counter = 1000;

int gw_create_gateway(const char *name)
{
    if (!name || g_gateway_count >= MAX_API_GATEWAYS)
    {
        return -1;
    }

    APIGateway *gateway = &g_gateways[g_gateway_count];
    gateway->gateway_id = g_gateway_count + 1;
    strncpy(gateway->name, name, 63);

    gateway->backend_count = 0;
    gateway->route_count = 0;
    gateway->api_key_count = 0;
    gateway->request_count = 0;

    gateway->enable_authentication = 1;
    gateway->enable_rate_limiting = 1;
    gateway->enable_logging = 1;
    gateway->enable_metrics = 1;

    gateway->total_requests = 0;
    gateway->total_errors = 0;
    gateway->total_bytes_in = 0;
    gateway->total_bytes_out = 0;

    g_gateway_count++;
    return gateway->gateway_id;
}

int gw_delete_gateway(uint32_t gateway_id)
{
    if (gateway_id == 0 || gateway_id > g_gateway_count)
    {
        return -1;
    }

    for (uint32_t i = gateway_id - 1; i < g_gateway_count - 1; i++)
    {
        g_gateways[i] = g_gateways[i + 1];
    }
    g_gateway_count--;

    return 0;
}

int gw_add_backend(uint32_t gateway_id, const char *name, BackendType backend_type,
                   const char *address, uint16_t port)
{
    if (gateway_id == 0 || gateway_id > g_gateway_count || !name || !address)
    {
        return -1;
    }

    APIGateway *gateway = &g_gateways[gateway_id - 1];
    if (gateway->backend_count >= MAX_BACKENDS)
    {
        return -1;
    }

    Backend *backend = &gateway->backends[gateway->backend_count];
    backend->backend_id = gateway->backend_count + 1;
    strncpy(backend->name, name, 63);
    backend->backend_type = backend_type;
    strncpy(backend->address, address, 255);
    backend->port = port;
    backend->healthy = 1;
    backend->request_count = 0;
    backend->error_count = 0;
    backend->avg_latency_ms = 0.0f;

    gateway->backend_count++;

    return backend->backend_id;
}

int gw_remove_backend(uint32_t gateway_id, uint32_t backend_id)
{
    if (gateway_id == 0 || gateway_id > g_gateway_count)
    {
        return -1;
    }

    APIGateway *gateway = &g_gateways[gateway_id - 1];

    for (uint32_t i = 0; i < gateway->backend_count; i++)
    {
        if (gateway->backends[i].backend_id == backend_id)
        {
            for (uint32_t j = i; j < gateway->backend_count - 1; j++)
            {
                gateway->backends[j] = gateway->backends[j + 1];
            }
            gateway->backend_count--;
            return 0;
        }
    }

    return -1;
}

int gw_update_backend_health(uint32_t gateway_id, uint32_t backend_id, uint32_t healthy)
{
    if (gateway_id == 0 || gateway_id > g_gateway_count)
    {
        return -1;
    }

    APIGateway *gateway = &g_gateways[gateway_id - 1];

    for (uint32_t i = 0; i < gateway->backend_count; i++)
    {
        if (gateway->backends[i].backend_id == backend_id)
        {
            gateway->backends[i].healthy = healthy;
            return 0;
        }
    }

    return -1;
}

int gw_get_backend_stats(uint32_t gateway_id, uint32_t backend_id,
                         uint64_t *request_count, float *avg_latency)
{
    if (gateway_id == 0 || gateway_id > g_gateway_count || !request_count || !avg_latency)
    {
        return -1;
    }

    APIGateway *gateway = &g_gateways[gateway_id - 1];

    for (uint32_t i = 0; i < gateway->backend_count; i++)
    {
        if (gateway->backends[i].backend_id == backend_id)
        {
            *request_count = gateway->backends[i].request_count;
            *avg_latency = gateway->backends[i].avg_latency_ms;
            return 0;
        }
    }

    return -1;
}

int gw_add_route(uint32_t gateway_id, const char *path, RouteType route_type,
                 const char *method, AuthType auth_type)
{
    if (gateway_id == 0 || gateway_id > g_gateway_count || !path || !method)
    {
        return -1;
    }

    APIGateway *gateway = &g_gateways[gateway_id - 1];
    if (gateway->route_count >= MAX_ROUTES)
    {
        return -1;
    }

    APIRoute *route = &gateway->routes[gateway->route_count];
    route->route_id = gateway->route_count + 1;
    strncpy(route->path, path, 255);
    route->route_type = route_type;
    strncpy(route->method, method, 15);
    route->auth_type = auth_type;
    route->backend_count = 0;
    route->rate_limit_requests = 1000;
    route->rate_limit_window_sec = 60;
    route->timeout_ms = 30000;
    route->retry_count = 3;
    route->is_enabled = 1;

    gateway->route_count++;

    return route->route_id;
}

int gw_remove_route(uint32_t gateway_id, uint32_t route_id)
{
    if (gateway_id == 0 || gateway_id > g_gateway_count)
    {
        return -1;
    }

    APIGateway *gateway = &g_gateways[gateway_id - 1];

    for (uint32_t i = 0; i < gateway->route_count; i++)
    {
        if (gateway->routes[i].route_id == route_id)
        {
            for (uint32_t j = i; j < gateway->route_count - 1; j++)
            {
                gateway->routes[j] = gateway->routes[j + 1];
            }
            gateway->route_count--;
            return 0;
        }
    }

    return -1;
}

int gw_add_backend_to_route(uint32_t gateway_id, uint32_t route_id, uint32_t backend_id)
{
    if (gateway_id == 0 || gateway_id > g_gateway_count)
    {
        return -1;
    }

    APIGateway *gateway = &g_gateways[gateway_id - 1];

    for (uint32_t i = 0; i < gateway->route_count; i++)
    {
        if (gateway->routes[i].route_id == route_id)
        {
            if (gateway->routes[i].backend_count >= 16)
            {
                return -1;
            }

            gateway->routes[i].backend_ids[gateway->routes[i].backend_count++] = backend_id;
            return 0;
        }
    }

    return -1;
}

int gw_remove_backend_from_route(uint32_t gateway_id, uint32_t route_id, uint32_t backend_id)
{
    if (gateway_id == 0 || gateway_id > g_gateway_count)
    {
        return -1;
    }

    APIGateway *gateway = &g_gateways[gateway_id - 1];

    for (uint32_t i = 0; i < gateway->route_count; i++)
    {
        if (gateway->routes[i].route_id == route_id)
        {
            for (uint32_t j = 0; j < gateway->routes[i].backend_count; j++)
            {
                if (gateway->routes[i].backend_ids[j] == backend_id)
                {
                    for (uint32_t k = j; k < gateway->routes[i].backend_count - 1; k++)
                    {
                        gateway->routes[i].backend_ids[k] = gateway->routes[i].backend_ids[k + 1];
                    }
                    gateway->routes[i].backend_count--;
                    return 0;
                }
            }
        }
    }

    return -1;
}

int gw_set_route_rate_limit(uint32_t gateway_id, uint32_t route_id, uint32_t requests, uint32_t window_sec)
{
    if (gateway_id == 0 || gateway_id > g_gateway_count)
    {
        return -1;
    }

    APIGateway *gateway = &g_gateways[gateway_id - 1];

    for (uint32_t i = 0; i < gateway->route_count; i++)
    {
        if (gateway->routes[i].route_id == route_id)
        {
            gateway->routes[i].rate_limit_requests = requests;
            gateway->routes[i].rate_limit_window_sec = window_sec;
            return 0;
        }
    }

    return -1;
}

int gw_set_route_timeout(uint32_t gateway_id, uint32_t route_id, uint32_t timeout_ms)
{
    if (gateway_id == 0 || gateway_id > g_gateway_count)
    {
        return -1;
    }

    APIGateway *gateway = &g_gateways[gateway_id - 1];

    for (uint32_t i = 0; i < gateway->route_count; i++)
    {
        if (gateway->routes[i].route_id == route_id)
        {
            gateway->routes[i].timeout_ms = timeout_ms;
            return 0;
        }
    }

    return -1;
}

int gw_issue_api_key(uint32_t gateway_id, const char *client_name, uint32_t rate_limit_rps)
{
    if (gateway_id == 0 || gateway_id > g_gateway_count || !client_name)
    {
        return -1;
    }

    APIGateway *gateway = &g_gateways[gateway_id - 1];
    if (gateway->api_key_count >= MAX_API_KEYS)
    {
        return -1;
    }

    APIKey *api_key = &gateway->api_keys[gateway->api_key_count];
    api_key->api_key_id = g_api_key_id_counter++;
    snprintf(api_key->api_key, 127, "key_%lu_%u", api_key->api_key_id, gateway_id);
    strncpy(api_key->client_name, client_name, 63);
    api_key->is_active = 1;
    api_key->created_at = 0;
    api_key->expires_at = 0;
    api_key->rate_limit_rps = rate_limit_rps;

    gateway->api_key_count++;

    return api_key->api_key_id;
}

int gw_revoke_api_key(uint32_t gateway_id, uint64_t api_key_id)
{
    if (gateway_id == 0 || gateway_id > g_gateway_count)
    {
        return -1;
    }

    APIGateway *gateway = &g_gateways[gateway_id - 1];

    for (uint32_t i = 0; i < gateway->api_key_count; i++)
    {
        if (gateway->api_keys[i].api_key_id == api_key_id)
        {
            gateway->api_keys[i].is_active = 0;
            return 0;
        }
    }

    return -1;
}

int gw_validate_api_key(uint32_t gateway_id, const char *api_key, uint32_t *is_valid)
{
    if (gateway_id == 0 || gateway_id > g_gateway_count || !api_key || !is_valid)
    {
        return -1;
    }

    APIGateway *gateway = &g_gateways[gateway_id - 1];
    *is_valid = 0;

    for (uint32_t i = 0; i < gateway->api_key_count; i++)
    {
        if (strcmp(gateway->api_keys[i].api_key, api_key) == 0 && gateway->api_keys[i].is_active)
        {
            *is_valid = 1;
            return 0;
        }
    }

    return 0;
}

int gw_handle_request(uint32_t gateway_id, const char *path, const char *method,
                      const char *client_ip, uint32_t *route_id, uint32_t *backend_id)
{
    if (gateway_id == 0 || gateway_id > g_gateway_count || !path || !method || !client_ip)
    {
        return -1;
    }

    APIGateway *gateway = &g_gateways[gateway_id - 1];

    for (uint32_t i = 0; i < gateway->route_count; i++)
    {
        if (gateway->routes[i].is_enabled &&
            strcmp(gateway->routes[i].path, path) == 0 &&
            strcmp(gateway->routes[i].method, method) == 0)
        {

            *route_id = gateway->routes[i].route_id;

            if (gateway->routes[i].backend_count > 0)
            {
                uint32_t healthy_count = 0;
                for (uint32_t j = 0; j < gateway->routes[i].backend_count; j++)
                {
                    uint32_t bid = gateway->routes[i].backend_ids[j];
                    for (uint32_t k = 0; k < gateway->backend_count; k++)
                    {
                        if (gateway->backends[k].backend_id == bid && gateway->backends[k].healthy)
                        {
                            healthy_count++;
                            if (healthy_count == 1)
                            {
                                *backend_id = bid;
                            }
                        }
                    }
                }

                if (healthy_count == 0 && gateway->routes[i].backend_count > 0)
                {
                    *backend_id = gateway->routes[i].backend_ids[0];
                }
            }

            return 0;
        }
    }

    return -1;
}

int gw_log_request(uint32_t gateway_id, uint64_t request_id, uint32_t status_code,
                   uint64_t latency_ms, uint64_t request_bytes, uint64_t response_bytes)
{
    if (gateway_id == 0 || gateway_id > g_gateway_count)
    {
        return -1;
    }

    APIGateway *gateway = &g_gateways[gateway_id - 1];

    if (gateway->request_count < MAX_ACTIVE_REQUESTS)
    {
        GatewayRequest *req = &gateway->requests[gateway->request_count];
        req->request_id = request_id;
        req->status_code = status_code;
        req->response_time = latency_ms;
        req->request_bytes = request_bytes;
        req->response_bytes = response_bytes;

        gateway->request_count++;
    }

    gateway->total_requests++;
    gateway->total_bytes_in += request_bytes;
    gateway->total_bytes_out += response_bytes;

    if (status_code >= 400)
    {
        gateway->total_errors++;
    }

    return 0;
}

int gw_enable_authentication(uint32_t gateway_id, uint32_t enable)
{
    if (gateway_id == 0 || gateway_id > g_gateway_count)
    {
        return -1;
    }

    g_gateways[gateway_id - 1].enable_authentication = enable;
    return 0;
}

int gw_enable_rate_limiting(uint32_t gateway_id, uint32_t enable)
{
    if (gateway_id == 0 || gateway_id > g_gateway_count)
    {
        return -1;
    }

    g_gateways[gateway_id - 1].enable_rate_limiting = enable;
    return 0;
}

int gw_enable_logging(uint32_t gateway_id, uint32_t enable)
{
    if (gateway_id == 0 || gateway_id > g_gateway_count)
    {
        return -1;
    }

    g_gateways[gateway_id - 1].enable_logging = enable;
    return 0;
}

int gw_enable_metrics(uint32_t gateway_id, uint32_t enable)
{
    if (gateway_id == 0 || gateway_id > g_gateway_count)
    {
        return -1;
    }

    g_gateways[gateway_id - 1].enable_metrics = enable;
    return 0;
}

uint64_t gw_get_total_requests(uint32_t gateway_id)
{
    if (gateway_id == 0 || gateway_id > g_gateway_count)
    {
        return 0;
    }

    return g_gateways[gateway_id - 1].total_requests;
}

uint64_t gw_get_total_errors(uint32_t gateway_id)
{
    if (gateway_id == 0 || gateway_id > g_gateway_count)
    {
        return 0;
    }

    return g_gateways[gateway_id - 1].total_errors;
}

uint64_t gw_get_bytes_in(uint32_t gateway_id)
{
    if (gateway_id == 0 || gateway_id > g_gateway_count)
    {
        return 0;
    }

    return g_gateways[gateway_id - 1].total_bytes_in;
}

uint64_t gw_get_bytes_out(uint32_t gateway_id)
{
    if (gateway_id == 0 || gateway_id > g_gateway_count)
    {
        return 0;
    }

    return g_gateways[gateway_id - 1].total_bytes_out;
}

int gw_export_metrics(uint32_t gateway_id, const char *format)
{
    if (gateway_id == 0 || gateway_id > g_gateway_count || !format)
    {
        return -1;
    }

    return 0;
}

APIGateway *gw_get_gateway_info(uint32_t gateway_id)
{
    if (gateway_id == 0 || gateway_id > g_gateway_count)
    {
        return NULL;
    }

    return &g_gateways[gateway_id - 1];
}
