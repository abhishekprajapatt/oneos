#include "load_balancer.h"
#include <stdlib.h>
#include <string.h>

static LoadBalancer *global_load_balancer = NULL;
static uint32_t next_server_id = 1;
static uint32_t next_request_id = 1;
static uint32_t next_pool_id = 1;

LoadBalancer *load_balancer_init(void)
{
    LoadBalancer *balancer = (LoadBalancer *)malloc(sizeof(LoadBalancer));
    if (!balancer)
        return NULL;

    memset(balancer, 0, sizeof(LoadBalancer));
    balancer->balancer_id = 1;
    balancer->pool_count = 0;
    balancer->total_servers = 0;
    balancer->healthy_servers = 0;
    balancer->total_bytes_processed = 0;
    balancer->total_connections = 0;

    global_load_balancer = balancer;
    return balancer;
}

int load_balancer_destroy(LoadBalancer *balancer)
{
    if (!balancer)
        return -1;

    free(balancer);
    global_load_balancer = NULL;
    return 0;
}

uint32_t load_balancer_pool_create(LoadBalancer *balancer, const char *pool_name,
                                   LoadBalancingAlgorithm algorithm)
{
    if (!balancer || balancer->pool_count >= MAX_LOAD_BALANCER_POOLS)
        return 0;

    LoadBalancerPool *pool = &balancer->pools[balancer->pool_count];
    memset(pool, 0, sizeof(LoadBalancerPool));

    pool->pool_id = next_pool_id++;

    if (pool_name)
    {
        strncpy(pool->pool_name, pool_name, sizeof(pool->pool_name) - 1);
    }

    pool->server_count = 0;
    pool->algorithm = algorithm;
    pool->current_server_index = 0;
    pool->request_count = 0;
    pool->total_requests = 0;
    pool->successful_requests = 0;
    pool->failed_requests = 0;
    pool->pool_throughput_mbps = 0.0f;

    balancer->pool_count++;
    return pool->pool_id;
}

int load_balancer_pool_destroy(LoadBalancer *balancer, uint32_t pool_id)
{
    if (!balancer || pool_id == 0)
        return -1;

    for (uint32_t i = 0; i < balancer->pool_count; i++)
    {
        if (balancer->pools[i].pool_id == pool_id)
        {
            memmove(&balancer->pools[i], &balancer->pools[i + 1],
                    sizeof(LoadBalancerPool) * (balancer->pool_count - i - 1));
            balancer->pool_count--;
            return 0;
        }
    }

    return -1;
}

int load_balancer_add_server(LoadBalancer *balancer, uint32_t pool_id,
                             uint32_t server_ip, uint16_t server_port, uint32_t weight)
{
    if (!balancer || pool_id == 0)
        return -1;

    for (uint32_t i = 0; i < balancer->pool_count; i++)
    {
        if (balancer->pools[i].pool_id == pool_id)
        {
            LoadBalancerPool *pool = &balancer->pools[i];

            if (pool->server_count >= MAX_BACKEND_SERVERS)
                return -1;

            BackendServer *server = &pool->servers[pool->server_count];
            memset(server, 0, sizeof(BackendServer));

            server->server_id = next_server_id++;
            server->server_ip = server_ip;
            server->server_port = server_port;
            server->state = SERVER_STATE_HEALTHY;
            server->weight = (weight > 0) ? weight : 1;
            server->active_connections = 0;
            server->total_requests = 0;
            server->bytes_sent = 0;
            server->bytes_received = 0;
            server->response_time_avg_ms = 0.0f;
            server->cpu_usage = 0.0f;
            server->memory_usage = 0.0f;

            pool->server_count++;
            balancer->total_servers++;
            balancer->healthy_servers++;

            return server->server_id;
        }
    }

    return -1;
}

int load_balancer_remove_server(LoadBalancer *balancer, uint32_t pool_id, uint32_t server_id)
{
    if (!balancer || pool_id == 0 || server_id == 0)
        return -1;

    for (uint32_t i = 0; i < balancer->pool_count; i++)
    {
        if (balancer->pools[i].pool_id == pool_id)
        {
            LoadBalancerPool *pool = &balancer->pools[i];

            for (uint32_t j = 0; j < pool->server_count; j++)
            {
                if (pool->servers[j].server_id == server_id)
                {
                    memmove(&pool->servers[j], &pool->servers[j + 1],
                            sizeof(BackendServer) * (pool->server_count - j - 1));
                    pool->server_count--;
                    balancer->total_servers--;
                    if (pool->servers[j].state == SERVER_STATE_HEALTHY)
                        balancer->healthy_servers--;
                    return 0;
                }
            }
        }
    }

    return -1;
}

int load_balancer_route_request(LoadBalancer *balancer, uint32_t pool_id,
                                uint32_t source_ip, uint16_t source_port)
{
    if (!balancer || pool_id == 0)
        return -1;

    for (uint32_t i = 0; i < balancer->pool_count; i++)
    {
        if (balancer->pools[i].pool_id == pool_id)
        {
            LoadBalancerPool *pool = &balancer->pools[i];

            int server_idx = load_balancer_select_server(balancer, pool_id);
            if (server_idx < 0)
                return -1;

            pool->total_requests++;
            pool->successful_requests++;
            balancer->total_connections++;

            return server_idx;
        }
    }

    return -1;
}

int load_balancer_select_server(LoadBalancer *balancer, uint32_t pool_id)
{
    if (!balancer || pool_id == 0)
        return -1;

    for (uint32_t i = 0; i < balancer->pool_count; i++)
    {
        if (balancer->pools[i].pool_id == pool_id)
        {
            LoadBalancerPool *pool = &balancer->pools[i];

            if (pool->server_count == 0)
                return -1;

            int selected_server = -1;

            switch (pool->algorithm)
            {
            case LB_ALGORITHM_ROUND_ROBIN:
                selected_server = pool->current_server_index % pool->server_count;
                pool->current_server_index++;
                break;

            case LB_ALGORITHM_LEAST_CONN:
            {
                uint32_t min_conn = UINT32_MAX;
                for (uint32_t j = 0; j < pool->server_count; j++)
                {
                    if (pool->servers[j].state == SERVER_STATE_HEALTHY)
                    {
                        if (pool->servers[j].active_connections < min_conn)
                        {
                            min_conn = pool->servers[j].active_connections;
                            selected_server = j;
                        }
                    }
                }
                break;
            }

            case LB_ALGORITHM_WEIGHTED:
            {
                uint32_t total_weight = 0;
                for (uint32_t j = 0; j < pool->server_count; j++)
                {
                    if (pool->servers[j].state == SERVER_STATE_HEALTHY)
                    {
                        total_weight += pool->servers[j].weight;
                    }
                }
                if (total_weight > 0)
                {
                    selected_server = pool->current_server_index % total_weight;
                    pool->current_server_index++;
                }
                break;
            }

            case LB_ALGORITHM_RANDOM:
                selected_server = pool->current_server_index % pool->server_count;
                pool->current_server_index++;
                break;

            default:
                selected_server = 0;
                break;
            }

            if (selected_server >= 0 && selected_server < (int)pool->server_count)
            {
                pool->servers[selected_server].active_connections++;
                pool->servers[selected_server].total_requests++;
                return selected_server;
            }

            return -1;
        }
    }

    return -1;
}

int load_balancer_update_server_state(LoadBalancer *balancer, uint32_t pool_id,
                                      uint32_t server_id, ServerState state)
{
    if (!balancer || pool_id == 0 || server_id == 0)
        return -1;

    for (uint32_t i = 0; i < balancer->pool_count; i++)
    {
        if (balancer->pools[i].pool_id == pool_id)
        {
            LoadBalancerPool *pool = &balancer->pools[i];

            for (uint32_t j = 0; j < pool->server_count; j++)
            {
                if (pool->servers[j].server_id == server_id)
                {
                    ServerState old_state = pool->servers[j].state;
                    pool->servers[j].state = state;

                    if (old_state == SERVER_STATE_HEALTHY && state != SERVER_STATE_HEALTHY)
                    {
                        balancer->healthy_servers--;
                    }
                    else if (old_state != SERVER_STATE_HEALTHY && state == SERVER_STATE_HEALTHY)
                    {
                        balancer->healthy_servers++;
                    }

                    return 0;
                }
            }
        }
    }

    return -1;
}

int load_balancer_update_server_metrics(LoadBalancer *balancer, uint32_t pool_id,
                                        uint32_t server_id, float cpu, float memory)
{
    if (!balancer || pool_id == 0 || server_id == 0)
        return -1;

    for (uint32_t i = 0; i < balancer->pool_count; i++)
    {
        if (balancer->pools[i].pool_id == pool_id)
        {
            LoadBalancerPool *pool = &balancer->pools[i];

            for (uint32_t j = 0; j < pool->server_count; j++)
            {
                if (pool->servers[j].server_id == server_id)
                {
                    pool->servers[j].cpu_usage = cpu;
                    pool->servers[j].memory_usage = memory;
                    return 0;
                }
            }
        }
    }

    return -1;
}

int load_balancer_detect_unhealthy_servers(LoadBalancer *balancer, uint32_t pool_id)
{
    if (!balancer || pool_id == 0)
        return 0;

    uint32_t unhealthy_count = 0;

    for (uint32_t i = 0; i < balancer->pool_count; i++)
    {
        if (balancer->pools[i].pool_id == pool_id)
        {
            LoadBalancerPool *pool = &balancer->pools[i];

            for (uint32_t j = 0; j < pool->server_count; j++)
            {
                if (pool->servers[j].cpu_usage > 90.0f || pool->servers[j].memory_usage > 85.0f)
                {
                    if (pool->servers[j].state == SERVER_STATE_HEALTHY)
                    {
                        pool->servers[j].state = SERVER_STATE_DEGRADED;
                        unhealthy_count++;
                    }
                }
            }
        }
    }

    return unhealthy_count;
}

int load_balancer_rebalance_pool(LoadBalancer *balancer, uint32_t pool_id)
{
    if (!balancer || pool_id == 0)
        return -1;

    return 0;
}

int load_balancer_enable_sticky_sessions(LoadBalancer *balancer, uint32_t pool_id)
{
    if (!balancer || pool_id == 0)
        return -1;

    return 0;
}

int load_balancer_disable_sticky_sessions(LoadBalancer *balancer, uint32_t pool_id)
{
    if (!balancer || pool_id == 0)
        return -1;

    return 0;
}

int load_balancer_drain_server(LoadBalancer *balancer, uint32_t pool_id, uint32_t server_id)
{
    if (!balancer || pool_id == 0 || server_id == 0)
        return -1;

    for (uint32_t i = 0; i < balancer->pool_count; i++)
    {
        if (balancer->pools[i].pool_id == pool_id)
        {
            LoadBalancerPool *pool = &balancer->pools[i];

            for (uint32_t j = 0; j < pool->server_count; j++)
            {
                if (pool->servers[j].server_id == server_id)
                {
                    pool->servers[j].state = SERVER_STATE_DRAINING;
                    return 0;
                }
            }
        }
    }

    return -1;
}

int load_balancer_get_metrics(LoadBalancer *balancer, LoadBalancerMetrics *metrics)
{
    if (!balancer || !metrics)
        return -1;

    uint32_t total_requests = 0;
    uint32_t successful_requests = 0;
    float total_response_time = 0.0f;

    for (uint32_t i = 0; i < balancer->pool_count; i++)
    {
        LoadBalancerPool *pool = &balancer->pools[i];
        total_requests += pool->total_requests;
        successful_requests += pool->successful_requests;

        for (uint32_t j = 0; j < pool->server_count; j++)
        {
            total_response_time += pool->servers[j].response_time_avg_ms;
        }
    }

    metrics->balancer_id = balancer->balancer_id;
    metrics->total_pools = balancer->pool_count;
    metrics->total_servers = balancer->total_servers;
    metrics->healthy_servers = balancer->healthy_servers;
    metrics->total_requests = total_requests;
    metrics->successful_requests = successful_requests;

    if (total_requests > 0)
    {
        metrics->success_rate = (float)successful_requests / total_requests;
    }
    else
    {
        metrics->success_rate = 0.0f;
    }

    if (balancer->total_servers > 0)
    {
        metrics->average_response_time_ms = total_response_time / balancer->total_servers;
    }
    else
    {
        metrics->average_response_time_ms = 0.0f;
    }

    return 0;
}
