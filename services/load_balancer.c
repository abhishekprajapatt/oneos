#include "load_balancer.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static LoadBalancer g_load_balancers[MAX_LOAD_BALANCERS] = {0};
static uint32_t g_lb_count = 0;
static ClientConnection g_connections[4096] = {0};
static uint32_t g_connection_count = 0;

int lb_create(const char *name, LoadBalancingAlgorithm algo, uint32_t listen_port)
{
    if (!name || g_lb_count >= MAX_LOAD_BALANCERS)
    {
        return -1;
    }

    LoadBalancer *lb = &g_load_balancers[g_lb_count];
    lb->lb_id = g_lb_count + 1;
    strncpy(lb->name, name, 63);
    lb->algorithm = algo;
    lb->listen_port = listen_port;
    lb->server_count = 0;
    lb->current_server_idx = 0;
    lb->health_check_interval = 30;
    lb->health_check_timeout = 5;
    lb->total_requests = 0;
    lb->total_connections = 0;
    lb->bytes_processed = 0;

    g_lb_count++;
    return lb->lb_id;
}

int lb_add_backend_server(uint32_t lb_id, uint32_t ip, uint16_t port, uint16_t weight)
{
    if (lb_id == 0 || lb_id > g_lb_count)
    {
        return -1;
    }

    LoadBalancer *lb = &g_load_balancers[lb_id - 1];
    if (lb->server_count >= MAX_BACKEND_SERVERS)
    {
        return -1;
    }

    BackendServer *server = &lb->servers[lb->server_count];
    server->server_id = lb->server_count + 1;
    server->ip_address = ip;
    server->port = port;
    server->weight = weight > 0 ? weight : 1;
    server->state = SERVER_ACTIVE;
    server->current_connections = 0;
    server->max_connections = MAX_CONNECTIONS_PER_SERVER;
    server->total_requests = 0;
    server->total_bytes_in = 0;
    server->total_bytes_out = 0;
    server->avg_response_time_ms = 0;
    server->consecutive_failures = 0;

    lb->server_count++;
    return server->server_id;
}

int lb_remove_backend_server(uint32_t lb_id, uint32_t server_id)
{
    if (lb_id == 0 || lb_id > g_lb_count)
    {
        return -1;
    }

    LoadBalancer *lb = &g_load_balancers[lb_id - 1];
    if (server_id == 0 || server_id > lb->server_count)
    {
        return -1;
    }

    for (uint32_t i = server_id - 1; i < lb->server_count - 1; i++)
    {
        lb->servers[i] = lb->servers[i + 1];
    }
    lb->server_count--;

    return 0;
}

int lb_set_server_weight(uint32_t lb_id, uint32_t server_id, uint16_t weight)
{
    if (lb_id == 0 || lb_id > g_lb_count || weight == 0)
    {
        return -1;
    }

    LoadBalancer *lb = &g_load_balancers[lb_id - 1];
    if (server_id == 0 || server_id > lb->server_count)
    {
        return -1;
    }

    lb->servers[server_id - 1].weight = weight;
    return 0;
}

int lb_mark_server_draining(uint32_t lb_id, uint32_t server_id)
{
    if (lb_id == 0 || lb_id > g_lb_count)
    {
        return -1;
    }

    LoadBalancer *lb = &g_load_balancers[lb_id - 1];
    if (server_id == 0 || server_id > lb->server_count)
    {
        return -1;
    }

    lb->servers[server_id - 1].state = SERVER_DRAINING;
    return 0;
}

int lb_mark_server_down(uint32_t lb_id, uint32_t server_id)
{
    if (lb_id == 0 || lb_id > g_lb_count)
    {
        return -1;
    }

    LoadBalancer *lb = &g_load_balancers[lb_id - 1];
    if (server_id == 0 || server_id > lb->server_count)
    {
        return -1;
    }

    lb->servers[server_id - 1].state = SERVER_DOWN;
    lb->servers[server_id - 1].consecutive_failures++;
    return 0;
}

int lb_restore_server(uint32_t lb_id, uint32_t server_id)
{
    if (lb_id == 0 || lb_id > g_lb_count)
    {
        return -1;
    }

    LoadBalancer *lb = &g_load_balancers[lb_id - 1];
    if (server_id == 0 || server_id > lb->server_count)
    {
        return -1;
    }

    lb->servers[server_id - 1].state = SERVER_ACTIVE;
    lb->servers[server_id - 1].consecutive_failures = 0;
    return 0;
}

ServerState lb_get_server_state(uint32_t lb_id, uint32_t server_id)
{
    if (lb_id == 0 || lb_id > g_lb_count)
    {
        return SERVER_DOWN;
    }

    LoadBalancer *lb = &g_load_balancers[lb_id - 1];
    if (server_id == 0 || server_id > lb->server_count)
    {
        return SERVER_DOWN;
    }

    return lb->servers[server_id - 1].state;
}

int lb_set_health_check(uint32_t lb_id, HealthCheckType type, uint32_t interval, uint32_t timeout)
{
    if (lb_id == 0 || lb_id > g_lb_count)
    {
        return -1;
    }

    LoadBalancer *lb = &g_load_balancers[lb_id - 1];
    lb->health_check_type = type;
    lb->health_check_interval = interval;
    lb->health_check_timeout = timeout;
    return 0;
}

int lb_perform_health_check(uint32_t lb_id, uint32_t server_id)
{
    if (lb_id == 0 || lb_id > g_lb_count)
    {
        return -1;
    }

    LoadBalancer *lb = &g_load_balancers[lb_id - 1];
    if (server_id == 0 || server_id > lb->server_count)
    {
        return -1;
    }

    BackendServer *server = &lb->servers[server_id - 1];
    server->last_health_check = time(NULL);

    if (server->current_connections < server->max_connections)
    {
        server->consecutive_failures = 0;
        server->state = SERVER_ACTIVE;
        return 1;
    }

    return 0;
}

int lb_health_check_all_servers(uint32_t lb_id)
{
    if (lb_id == 0 || lb_id > g_lb_count)
    {
        return -1;
    }

    LoadBalancer *lb = &g_load_balancers[lb_id - 1];
    int healthy_count = 0;

    for (uint32_t i = 0; i < lb->server_count; i++)
    {
        if (lb_perform_health_check(lb_id, lb->servers[i].server_id) > 0)
        {
            healthy_count++;
        }
    }

    return healthy_count;
}

uint32_t lb_select_backend_server(uint32_t lb_id, uint32_t client_ip, const void *request_data, size_t request_size)
{
    if (lb_id == 0 || lb_id > g_lb_count)
    {
        return 0;
    }

    LoadBalancer *lb = &g_load_balancers[lb_id - 1];
    uint32_t selected_server = 0;

    switch (lb->algorithm)
    {
    case ALGO_ROUND_ROBIN:
        selected_server = lb->servers[lb->current_server_idx % lb->server_count].server_id;
        lb->current_server_idx++;
        break;

    case ALGO_LEAST_CONNECTIONS:
    {
        uint32_t min_conns = UINT32_MAX;
        for (uint32_t i = 0; i < lb->server_count; i++)
        {
            if (lb->servers[i].state == SERVER_ACTIVE &&
                lb->servers[i].current_connections < min_conns)
            {
                min_conns = lb->servers[i].current_connections;
                selected_server = lb->servers[i].server_id;
            }
        }
    }
    break;

    case ALGO_IP_HASH:
        selected_server = (client_ip % lb->server_count) + 1;
        break;

    case ALGO_LEAST_RESPONSE_TIME:
    {
        uint32_t min_time = UINT32_MAX;
        for (uint32_t i = 0; i < lb->server_count; i++)
        {
            if (lb->servers[i].state == SERVER_ACTIVE &&
                lb->servers[i].avg_response_time_ms < min_time)
            {
                min_time = lb->servers[i].avg_response_time_ms;
                selected_server = lb->servers[i].server_id;
            }
        }
    }
    break;

    default:
        selected_server = lb->servers[0].server_id;
    }

    if (selected_server > 0 && selected_server <= lb->server_count)
    {
        lb->servers[selected_server - 1].current_connections++;
        lb->servers[selected_server - 1].total_requests++;
    }

    return selected_server;
}

int lb_route_request(uint32_t lb_id, uint32_t client_ip, const void *request_data, size_t request_size)
{
    if (lb_id == 0 || lb_id > g_lb_count)
    {
        return -1;
    }

    LoadBalancer *lb = &g_load_balancers[lb_id - 1];
    uint32_t server_id = lb_select_backend_server(lb_id, client_ip, request_data, request_size);

    if (server_id > 0)
    {
        lb->total_requests++;
        lb->bytes_processed += request_size;
        return server_id;
    }

    return -1;
}

int lb_handle_client_connection(uint32_t lb_id, uint64_t conn_id, uint32_t client_ip)
{
    if (lb_id == 0 || lb_id > g_lb_count)
    {
        return -1;
    }

    LoadBalancer *lb = &g_load_balancers[lb_id - 1];
    if (g_connection_count >= 4096)
    {
        return -1;
    }

    ClientConnection *conn = &g_connections[g_connection_count];
    conn->connection_id = conn_id;
    conn->client_ip = client_ip;
    conn->connection_start_time = time(NULL);
    conn->bytes_sent = 0;
    conn->bytes_received = 0;
    conn->response_time_ms = 0;

    lb->total_connections++;
    g_connection_count++;

    return 0;
}

int lb_close_client_connection(uint32_t lb_id, uint64_t conn_id)
{
    if (lb_id == 0 || lb_id > g_lb_count)
    {
        return -1;
    }

    LoadBalancer *lb = &g_load_balancers[lb_id - 1];

    for (uint32_t i = 0; i < g_connection_count; i++)
    {
        if (g_connections[i].connection_id == conn_id)
        {
            uint32_t server_id = g_connections[i].server_id;
            if (server_id > 0 && server_id <= lb->server_count)
            {
                if (lb->servers[server_id - 1].current_connections > 0)
                {
                    lb->servers[server_id - 1].current_connections--;
                }
            }

            for (uint32_t j = i; j < g_connection_count - 1; j++)
            {
                g_connections[j] = g_connections[j + 1];
            }
            g_connection_count--;
            return 0;
        }
    }

    return -1;
}

int lb_store_session_data(uint32_t lb_id, uint64_t conn_id, const void *data, size_t size)
{
    if (lb_id == 0 || lb_id > g_lb_count || !data || size > MAX_SESSION_DATA)
    {
        return -1;
    }

    for (uint32_t i = 0; i < g_connection_count; i++)
    {
        if (g_connections[i].connection_id == conn_id)
        {
            memcpy(g_connections[i].session_data, data, size);
            g_connections[i].session_data_size = size;
            return 0;
        }
    }

    return -1;
}

void *lb_retrieve_session_data(uint32_t lb_id, uint64_t conn_id, size_t *size)
{
    if (lb_id == 0 || lb_id > g_lb_count || !size)
    {
        return NULL;
    }

    for (uint32_t i = 0; i < g_connection_count; i++)
    {
        if (g_connections[i].connection_id == conn_id)
        {
            *size = g_connections[i].session_data_size;
            return g_connections[i].session_data;
        }
    }

    return NULL;
}

int lb_update_connection_stats(uint32_t lb_id, uint32_t server_id, uint64_t bytes_in, uint64_t bytes_out, uint32_t response_time)
{
    if (lb_id == 0 || lb_id > g_lb_count)
    {
        return -1;
    }

    LoadBalancer *lb = &g_load_balancers[lb_id - 1];
    if (server_id == 0 || server_id > lb->server_count)
    {
        return -1;
    }

    BackendServer *server = &lb->servers[server_id - 1];
    server->total_bytes_in += bytes_in;
    server->total_bytes_out += bytes_out;
    server->avg_response_time_ms = (server->avg_response_time_ms + response_time) / 2;

    lb->bytes_processed += bytes_in + bytes_out;

    return 0;
}

uint32_t lb_get_active_connections(uint32_t lb_id, uint32_t server_id)
{
    if (lb_id == 0 || lb_id > g_lb_count)
    {
        return 0;
    }

    LoadBalancer *lb = &g_load_balancers[lb_id - 1];
    if (server_id == 0 || server_id > lb->server_count)
    {
        return 0;
    }

    return lb->servers[server_id - 1].current_connections;
}

uint32_t lb_get_total_connections_lb(uint32_t lb_id)
{
    if (lb_id == 0 || lb_id > g_lb_count)
    {
        return 0;
    }

    return g_load_balancers[lb_id - 1].total_connections;
}

int lb_rebalance_load(uint32_t lb_id)
{
    if (lb_id == 0 || lb_id > g_lb_count)
    {
        return -1;
    }

    return 0;
}

float lb_get_server_utilization(uint32_t lb_id, uint32_t server_id)
{
    if (lb_id == 0 || lb_id > g_lb_count)
    {
        return 0.0f;
    }

    LoadBalancer *lb = &g_load_balancers[lb_id - 1];
    if (server_id == 0 || server_id > lb->server_count)
    {
        return 0.0f;
    }

    BackendServer *server = &lb->servers[server_id - 1];
    return (float)server->current_connections / (float)server->max_connections;
}

int lb_get_least_loaded_server(uint32_t lb_id)
{
    if (lb_id == 0 || lb_id > g_lb_count)
    {
        return -1;
    }

    LoadBalancer *lb = &g_load_balancers[lb_id - 1];
    uint32_t min_conns = UINT32_MAX;
    int least_loaded = -1;

    for (uint32_t i = 0; i < lb->server_count; i++)
    {
        if (lb->servers[i].state == SERVER_ACTIVE &&
            lb->servers[i].current_connections < min_conns)
        {
            min_conns = lb->servers[i].current_connections;
            least_loaded = lb->servers[i].server_id;
        }
    }

    return least_loaded;
}

LoadBalancer *lb_get_info(uint32_t lb_id)
{
    if (lb_id == 0 || lb_id > g_lb_count)
    {
        return NULL;
    }

    return &g_load_balancers[lb_id - 1];
}

uint64_t lb_get_total_requests(uint32_t lb_id)
{
    if (lb_id == 0 || lb_id > g_lb_count)
    {
        return 0;
    }

    return g_load_balancers[lb_id - 1].total_requests;
}

uint64_t lb_get_bytes_processed(uint32_t lb_id)
{
    if (lb_id == 0 || lb_id > g_lb_count)
    {
        return 0;
    }

    return g_load_balancers[lb_id - 1].bytes_processed;
}
