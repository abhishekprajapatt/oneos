#include "service_discovery.h"
#include <stdlib.h>
#include <string.h>

static ServiceDiscovery *global_service_discovery = NULL;
static uint32_t next_service_id = 1;
static uint32_t next_instance_id = 1;
static uint32_t next_endpoint_id = 1;

ServiceDiscovery *service_discovery_init(void)
{
    ServiceDiscovery *discovery = (ServiceDiscovery *)malloc(sizeof(ServiceDiscovery));
    if (!discovery)
        return NULL;

    memset(discovery, 0, sizeof(ServiceDiscovery));
    discovery->discovery_id = 1;
    discovery->service_count = 0;
    discovery->total_service_registrations = 0;
    discovery->active_instances = 0;
    discovery->service_discovery_calls = 0;

    global_service_discovery = discovery;
    return discovery;
}

int service_discovery_destroy(ServiceDiscovery *discovery)
{
    if (!discovery)
        return -1;

    free(discovery);
    global_service_discovery = NULL;
    return 0;
}

uint32_t service_create(ServiceDiscovery *discovery, const char *service_name)
{
    if (!discovery || discovery->service_count >= MAX_SERVICES || !service_name)
        return 0;

    Service *service = &discovery->services[discovery->service_count];
    memset(service, 0, sizeof(Service));

    service->service_id = next_service_id++;
    strncpy(service->service_name, service_name, sizeof(service->service_name) - 1);

    service->instance_count = 0;
    service->healthy_instances = 0;
    service->unhealthy_instances = 0;
    service->load_balancing_enabled = 0;
    service->health_check_enabled = 0;

    discovery->service_count++;

    return service->service_id;
}

int service_destroy(ServiceDiscovery *discovery, uint32_t service_id)
{
    if (!discovery || service_id == 0)
        return -1;

    for (uint32_t i = 0; i < discovery->service_count; i++)
    {
        if (discovery->services[i].service_id == service_id)
        {
            memmove(&discovery->services[i], &discovery->services[i + 1],
                    sizeof(Service) * (discovery->service_count - i - 1));
            discovery->service_count--;
            return 0;
        }
    }

    return -1;
}

uint32_t service_instance_register(ServiceDiscovery *discovery, uint32_t service_id,
                                   const char *instance_name)
{
    if (!discovery || service_id == 0 || !instance_name)
        return 0;

    for (uint32_t i = 0; i < discovery->service_count; i++)
    {
        if (discovery->services[i].service_id == service_id)
        {
            Service *service = &discovery->services[i];

            if (service->instance_count >= MAX_SERVICE_INSTANCES_PER_SERVICE)
                return 0;

            ServiceInstance *instance = &service->instances[service->instance_count];
            memset(instance, 0, sizeof(ServiceInstance));

            instance->instance_id = next_instance_id++;
            strncpy(instance->instance_name, instance_name, sizeof(instance->instance_name) - 1);

            instance->endpoint_count = 0;
            instance->health_status = SERVICE_HEALTH_UNKNOWN;
            instance->last_heartbeat_time = 0;
            instance->is_registered = 1;

            service->instance_count++;
            discovery->active_instances++;
            discovery->total_service_registrations++;

            return instance->instance_id;
        }
    }

    return 0;
}

int service_instance_deregister(ServiceDiscovery *discovery, uint32_t service_id,
                                uint32_t instance_id)
{
    if (!discovery || service_id == 0 || instance_id == 0)
        return -1;

    for (uint32_t i = 0; i < discovery->service_count; i++)
    {
        if (discovery->services[i].service_id == service_id)
        {
            Service *service = &discovery->services[i];

            for (uint32_t j = 0; j < service->instance_count; j++)
            {
                if (service->instances[j].instance_id == instance_id)
                {
                    memmove(&service->instances[j], &service->instances[j + 1],
                            sizeof(ServiceInstance) * (service->instance_count - j - 1));
                    service->instance_count--;
                    discovery->active_instances--;
                    return 0;
                }
            }
        }
    }

    return -1;
}

int service_instance_add_endpoint(ServiceDiscovery *discovery, uint32_t service_id,
                                  uint32_t instance_id, const char *host, uint32_t port,
                                  ServiceProtocol protocol)
{
    if (!discovery || service_id == 0 || instance_id == 0 || !host)
        return -1;

    for (uint32_t i = 0; i < discovery->service_count; i++)
    {
        if (discovery->services[i].service_id == service_id)
        {
            Service *service = &discovery->services[i];

            for (uint32_t j = 0; j < service->instance_count; j++)
            {
                if (service->instances[j].instance_id == instance_id)
                {
                    ServiceInstance *instance = &service->instances[j];

                    if (instance->endpoint_count >= MAX_SERVICE_ENDPOINTS_PER_INSTANCE)
                        return -1;

                    ServiceEndpoint *endpoint = &instance->endpoints[instance->endpoint_count];
                    endpoint->endpoint_id = next_endpoint_id++;
                    strncpy(endpoint->host, host, sizeof(endpoint->host) - 1);
                    endpoint->port = port;
                    endpoint->protocol = protocol;
                    endpoint->is_primary = (instance->endpoint_count == 0) ? 1 : 0;
                    endpoint->is_available = 1;

                    instance->endpoint_count++;
                    instance->health_status = SERVICE_HEALTH_UP;
                    service->healthy_instances++;

                    return 0;
                }
            }
        }
    }

    return -1;
}

int service_instance_update_health(ServiceDiscovery *discovery, uint32_t service_id,
                                   uint32_t instance_id, ServiceHealth health)
{
    if (!discovery || service_id == 0 || instance_id == 0)
        return -1;

    for (uint32_t i = 0; i < discovery->service_count; i++)
    {
        if (discovery->services[i].service_id == service_id)
        {
            Service *service = &discovery->services[i];

            for (uint32_t j = 0; j < service->instance_count; j++)
            {
                if (service->instances[j].instance_id == instance_id)
                {
                    ServiceInstance *instance = &service->instances[j];

                    if (instance->health_status == SERVICE_HEALTH_UP && health != SERVICE_HEALTH_UP)
                    {
                        service->healthy_instances--;
                        service->unhealthy_instances++;
                    }
                    else if (instance->health_status != SERVICE_HEALTH_UP && health == SERVICE_HEALTH_UP)
                    {
                        service->unhealthy_instances--;
                        service->healthy_instances++;
                    }

                    instance->health_status = health;
                    return 0;
                }
            }
        }
    }

    return -1;
}

int service_enable_load_balancing(ServiceDiscovery *discovery, uint32_t service_id)
{
    if (!discovery || service_id == 0)
        return -1;

    for (uint32_t i = 0; i < discovery->service_count; i++)
    {
        if (discovery->services[i].service_id == service_id)
        {
            discovery->services[i].load_balancing_enabled = 1;
            return 0;
        }
    }

    return -1;
}

int service_disable_load_balancing(ServiceDiscovery *discovery, uint32_t service_id)
{
    if (!discovery || service_id == 0)
        return -1;

    for (uint32_t i = 0; i < discovery->service_count; i++)
    {
        if (discovery->services[i].service_id == service_id)
        {
            discovery->services[i].load_balancing_enabled = 0;
            return 0;
        }
    }

    return -1;
}

uint32_t service_discover(ServiceDiscovery *discovery, const char *service_name,
                          uint32_t *instance_ids, uint32_t max_results)
{
    if (!discovery || !service_name || !instance_ids)
        return 0;

    uint32_t count = 0;

    for (uint32_t i = 0; i < discovery->service_count; i++)
    {
        if (strncmp(discovery->services[i].service_name, service_name,
                    sizeof(discovery->services[i].service_name)) == 0)
        {

            Service *service = &discovery->services[i];

            for (uint32_t j = 0; j < service->instance_count && count < max_results; j++)
            {
                instance_ids[count++] = service->instances[j].instance_id;
            }

            break;
        }
    }

    discovery->service_discovery_calls++;
    return count;
}

uint32_t service_discover_healthy(ServiceDiscovery *discovery, const char *service_name,
                                  uint32_t *instance_ids, uint32_t max_results)
{
    if (!discovery || !service_name || !instance_ids)
        return 0;

    uint32_t count = 0;

    for (uint32_t i = 0; i < discovery->service_count; i++)
    {
        if (strncmp(discovery->services[i].service_name, service_name,
                    sizeof(discovery->services[i].service_name)) == 0)
        {

            Service *service = &discovery->services[i];

            for (uint32_t j = 0; j < service->instance_count && count < max_results; j++)
            {
                if (service->instances[j].health_status == SERVICE_HEALTH_UP)
                {
                    instance_ids[count++] = service->instances[j].instance_id;
                }
            }

            break;
        }
    }

    discovery->service_discovery_calls++;
    return count;
}

int service_get_instance_endpoint(ServiceDiscovery *discovery, uint32_t service_id,
                                  uint32_t instance_id, uint32_t endpoint_index,
                                  char *host_buffer, uint32_t host_size, uint32_t *port)
{
    if (!discovery || service_id == 0 || instance_id == 0 || !host_buffer || !port)
        return -1;

    for (uint32_t i = 0; i < discovery->service_count; i++)
    {
        if (discovery->services[i].service_id == service_id)
        {
            Service *service = &discovery->services[i];

            for (uint32_t j = 0; j < service->instance_count; j++)
            {
                if (service->instances[j].instance_id == instance_id)
                {
                    ServiceInstance *instance = &service->instances[j];

                    if (endpoint_index < instance->endpoint_count)
                    {
                        strncpy(host_buffer, instance->endpoints[endpoint_index].host, host_size - 1);
                        *port = instance->endpoints[endpoint_index].port;
                        return 0;
                    }

                    return -1;
                }
            }
        }
    }

    return -1;
}

int service_discovery_get_metrics(ServiceDiscovery *discovery, ServiceDiscoveryMetrics *metrics)
{
    if (!discovery || !metrics)
        return -1;

    metrics->discovery_id = discovery->discovery_id;
    metrics->total_services = discovery->service_count;
    metrics->active_instances = discovery->active_instances;
    metrics->total_registrations = discovery->total_service_registrations;
    metrics->discovery_calls = discovery->service_discovery_calls;

    return 0;
}
