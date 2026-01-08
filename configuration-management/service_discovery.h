#ifndef SERVICE_DISCOVERY_H
#define SERVICE_DISCOVERY_H

#include <stdint.h>
#include <stddef.h>

#define MAX_SERVICES 128
#define MAX_SERVICE_INSTANCES_PER_SERVICE 32
#define MAX_SERVICE_ENDPOINTS_PER_INSTANCE 16

typedef enum
{
    SERVICE_HEALTH_UP,
    SERVICE_HEALTH_DOWN,
    SERVICE_HEALTH_UNKNOWN
} ServiceHealth;

typedef enum
{
    SERVICE_PROTOCOL_HTTP,
    SERVICE_PROTOCOL_HTTPS,
    SERVICE_PROTOCOL_GRPC,
    SERVICE_PROTOCOL_TCP,
    SERVICE_PROTOCOL_CUSTOM
} ServiceProtocol;

typedef struct
{
    uint32_t endpoint_id;
    char host[128];
    uint32_t port;
    ServiceProtocol protocol;
    uint8_t is_primary;
    uint8_t is_available;
} ServiceEndpoint;

typedef struct
{
    uint32_t instance_id;
    char instance_name[128];

    ServiceEndpoint endpoints[MAX_SERVICE_ENDPOINTS_PER_INSTANCE];
    uint32_t endpoint_count;

    ServiceHealth health_status;
    uint64_t last_heartbeat_time;
    uint8_t is_registered;
} ServiceInstance;

typedef struct
{
    uint32_t service_id;
    char service_name[128];

    ServiceInstance instances[MAX_SERVICE_INSTANCES_PER_SERVICE];
    uint32_t instance_count;

    uint32_t healthy_instances;
    uint32_t unhealthy_instances;

    uint8_t load_balancing_enabled;
    uint8_t health_check_enabled;
} Service;

typedef struct
{
    uint32_t discovery_id;

    Service services[MAX_SERVICES];
    uint32_t service_count;

    uint64_t total_service_registrations;
    uint32_t active_instances;

    uint64_t service_discovery_calls;
} ServiceDiscovery;

ServiceDiscovery *service_discovery_init(void);
int service_discovery_destroy(ServiceDiscovery *discovery);

uint32_t service_create(ServiceDiscovery *discovery, const char *service_name);
int service_destroy(ServiceDiscovery *discovery, uint32_t service_id);

uint32_t service_instance_register(ServiceDiscovery *discovery, uint32_t service_id,
                                   const char *instance_name);

int service_instance_deregister(ServiceDiscovery *discovery, uint32_t service_id,
                                uint32_t instance_id);

int service_instance_add_endpoint(ServiceDiscovery *discovery, uint32_t service_id,
                                  uint32_t instance_id, const char *host, uint32_t port,
                                  ServiceProtocol protocol);

int service_instance_update_health(ServiceDiscovery *discovery, uint32_t service_id,
                                   uint32_t instance_id, ServiceHealth health);

int service_enable_load_balancing(ServiceDiscovery *discovery, uint32_t service_id);

int service_disable_load_balancing(ServiceDiscovery *discovery, uint32_t service_id);

uint32_t service_discover(ServiceDiscovery *discovery, const char *service_name,
                          uint32_t *instance_ids, uint32_t max_results);

uint32_t service_discover_healthy(ServiceDiscovery *discovery, const char *service_name,
                                  uint32_t *instance_ids, uint32_t max_results);

int service_get_instance_endpoint(ServiceDiscovery *discovery, uint32_t service_id,
                                  uint32_t instance_id, uint32_t endpoint_index,
                                  char *host_buffer, uint32_t host_size, uint32_t *port);

typedef struct
{
    uint32_t discovery_id;
    uint32_t total_services;
    uint32_t active_instances;
    uint64_t total_registrations;
    uint64_t discovery_calls;
} ServiceDiscoveryMetrics;

int service_discovery_get_metrics(ServiceDiscovery *discovery, ServiceDiscoveryMetrics *metrics);

#endif
