#ifndef CLOUD_RESOURCE_MANAGER_H
#define CLOUD_RESOURCE_MANAGER_H

#include <stdint.h>
#include <stddef.h>

#define MAX_CLOUD_RESOURCES 4096
#define MAX_RESOURCE_TAGS 32
#define MAX_RESOURCE_PROPERTIES 128
#define MAX_RESOURCE_GROUPS 256

typedef enum
{
    RESOURCE_TYPE_COMPUTE,
    RESOURCE_TYPE_STORAGE,
    RESOURCE_TYPE_DATABASE,
    RESOURCE_TYPE_NETWORK,
    RESOURCE_TYPE_LOAD_BALANCER,
    RESOURCE_TYPE_CACHE,
    RESOURCE_TYPE_MESSAGE_QUEUE,
    RESOURCE_TYPE_CONTAINER_REGISTRY
} ResourceType;

typedef enum
{
    RESOURCE_STATE_PENDING,
    RESOURCE_STATE_PROVISIONING,
    RESOURCE_STATE_ACTIVE,
    RESOURCE_STATE_UPDATING,
    RESOURCE_STATE_DELETING,
    RESOURCE_STATE_DELETED,
    RESOURCE_STATE_ERROR
} ResourceState;

typedef struct
{
    char tag_key[64];
    char tag_value[256];
} ResourceTag;

typedef struct
{
    char property_key[64];
    char property_value[512];
} ResourceProperty;

typedef struct
{
    uint32_t resource_id;
    ResourceType resource_type;
    char resource_name[256];
    char resource_arn[512];

    ResourceState state;

    ResourceTag tags[MAX_RESOURCE_TAGS];
    uint32_t tag_count;

    ResourceProperty properties[MAX_RESOURCE_PROPERTIES];
    uint32_t property_count;

    uint64_t creation_time;
    uint64_t last_modified_time;

    uint8_t is_managed;
    uint8_t is_billable;
    uint64_t estimated_cost_per_month;
} CloudResource;

typedef struct
{
    uint32_t group_id;
    char group_name[256];

    uint32_t resource_ids[MAX_CLOUD_RESOURCES];
    uint32_t resource_count;

    uint64_t total_cost_per_month;
    uint8_t is_auto_managed;
} ResourceGroup;

typedef struct
{
    uint32_t manager_id;

    CloudResource resources[MAX_CLOUD_RESOURCES];
    uint32_t resource_count;

    ResourceGroup groups[MAX_RESOURCE_GROUPS];
    uint32_t group_count;

    uint64_t total_provisioned_resources;
    uint64_t total_cost_per_month;
    uint32_t active_resources;
    uint32_t pending_operations;
} CloudResourceManager;

CloudResourceManager *cloud_resource_manager_init(void);
int cloud_resource_manager_destroy(CloudResourceManager *manager);

uint32_t cloud_resource_create(CloudResourceManager *manager, ResourceType type,
                               const char *resource_name, const char *arn);

int cloud_resource_add_tag(CloudResourceManager *manager, uint32_t resource_id,
                           const char *tag_key, const char *tag_value);

int cloud_resource_set_property(CloudResourceManager *manager, uint32_t resource_id,
                                const char *property_key, const char *property_value);

int cloud_resource_transition_state(CloudResourceManager *manager, uint32_t resource_id,
                                    ResourceState new_state);

int cloud_resource_delete(CloudResourceManager *manager, uint32_t resource_id);

int cloud_resource_get_state(CloudResourceManager *manager, uint32_t resource_id,
                             ResourceState *state);

int cloud_resource_mark_billable(CloudResourceManager *manager, uint32_t resource_id,
                                 uint64_t cost_per_month);

int cloud_resource_set_cost_estimate(CloudResourceManager *manager, uint32_t resource_id,
                                     uint64_t estimated_cost_per_month);

uint32_t resource_group_create(CloudResourceManager *manager, const char *group_name);

int resource_group_add_resource(CloudResourceManager *manager, uint32_t group_id,
                                uint32_t resource_id);

int resource_group_remove_resource(CloudResourceManager *manager, uint32_t group_id,
                                   uint32_t resource_id);

int resource_group_enable_auto_management(CloudResourceManager *manager, uint32_t group_id);

uint32_t cloud_resource_find_by_tag(CloudResourceManager *manager, const char *tag_key,
                                    const char *tag_value, uint32_t *matching_resources,
                                    uint32_t max_results);

uint32_t cloud_resource_find_by_type(CloudResourceManager *manager, ResourceType type,
                                     uint32_t *matching_resources, uint32_t max_results);

int cloud_resource_calculate_group_cost(CloudResourceManager *manager, uint32_t group_id,
                                        uint64_t *total_cost);

int cloud_resource_manager_calculate_total_cost(CloudResourceManager *manager,
                                                uint64_t *total_cost);

int cloud_resource_manager_optimize_costs(CloudResourceManager *manager);

typedef struct
{
    uint32_t manager_id;
    uint32_t total_resources;
    uint32_t active_resources;
    uint32_t pending_operations;
    uint32_t total_groups;
    uint64_t total_cost_per_month;
    uint64_t total_provisioned;
    uint32_t resource_type_distribution[8];
} CloudResourceManagerMetrics;

int cloud_resource_manager_get_metrics(CloudResourceManager *manager,
                                       CloudResourceManagerMetrics *metrics);

#endif
