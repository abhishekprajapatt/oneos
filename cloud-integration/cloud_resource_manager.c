#include "cloud_resource_manager.h"
#include <stdlib.h>
#include <string.h>

static CloudResourceManager *global_cloud_resource_manager = NULL;
static uint32_t next_resource_id = 1;
static uint32_t next_group_id = 1;

CloudResourceManager *cloud_resource_manager_init(void)
{
    CloudResourceManager *manager = (CloudResourceManager *)malloc(sizeof(CloudResourceManager));
    if (!manager)
        return NULL;

    memset(manager, 0, sizeof(CloudResourceManager));
    manager->manager_id = 1;
    manager->resource_count = 0;
    manager->group_count = 0;
    manager->total_provisioned_resources = 0;
    manager->total_cost_per_month = 0;
    manager->active_resources = 0;
    manager->pending_operations = 0;

    global_cloud_resource_manager = manager;
    return manager;
}

int cloud_resource_manager_destroy(CloudResourceManager *manager)
{
    if (!manager)
        return -1;

    free(manager);
    global_cloud_resource_manager = NULL;
    return 0;
}

uint32_t cloud_resource_create(CloudResourceManager *manager, ResourceType type,
                               const char *resource_name, const char *arn)
{
    if (!manager || manager->resource_count >= MAX_CLOUD_RESOURCES || !resource_name || !arn)
        return 0;

    CloudResource *resource = &manager->resources[manager->resource_count];
    memset(resource, 0, sizeof(CloudResource));

    resource->resource_id = next_resource_id++;
    resource->resource_type = type;
    strncpy(resource->resource_name, resource_name, sizeof(resource->resource_name) - 1);
    strncpy(resource->resource_arn, arn, sizeof(resource->resource_arn) - 1);

    resource->state = RESOURCE_STATE_PENDING;
    resource->tag_count = 0;
    resource->property_count = 0;

    resource->creation_time = 0;
    resource->last_modified_time = 0;

    resource->is_managed = 1;
    resource->is_billable = 0;
    resource->estimated_cost_per_month = 0;

    manager->resource_count++;
    manager->total_provisioned_resources++;
    manager->pending_operations++;

    return resource->resource_id;
}

int cloud_resource_add_tag(CloudResourceManager *manager, uint32_t resource_id,
                           const char *tag_key, const char *tag_value)
{
    if (!manager || resource_id == 0 || !tag_key || !tag_value)
        return -1;

    for (uint32_t i = 0; i < manager->resource_count; i++)
    {
        if (manager->resources[i].resource_id == resource_id)
        {
            CloudResource *resource = &manager->resources[i];

            if (resource->tag_count >= MAX_RESOURCE_TAGS)
                return -1;

            ResourceTag *tag = &resource->tags[resource->tag_count];
            strncpy(tag->tag_key, tag_key, sizeof(tag->tag_key) - 1);
            strncpy(tag->tag_value, tag_value, sizeof(tag->tag_value) - 1);

            resource->tag_count++;
            return 0;
        }
    }

    return -1;
}

int cloud_resource_set_property(CloudResourceManager *manager, uint32_t resource_id,
                                const char *property_key, const char *property_value)
{
    if (!manager || resource_id == 0 || !property_key || !property_value)
        return -1;

    for (uint32_t i = 0; i < manager->resource_count; i++)
    {
        if (manager->resources[i].resource_id == resource_id)
        {
            CloudResource *resource = &manager->resources[i];

            if (resource->property_count >= MAX_RESOURCE_PROPERTIES)
                return -1;

            ResourceProperty *property = &resource->properties[resource->property_count];
            strncpy(property->property_key, property_key, sizeof(property->property_key) - 1);
            strncpy(property->property_value, property_value, sizeof(property->property_value) - 1);

            resource->property_count++;
            resource->last_modified_time = 0;
            return 0;
        }
    }

    return -1;
}

int cloud_resource_transition_state(CloudResourceManager *manager, uint32_t resource_id,
                                    ResourceState new_state)
{
    if (!manager || resource_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->resource_count; i++)
    {
        if (manager->resources[i].resource_id == resource_id)
        {
            CloudResource *resource = &manager->resources[i];
            resource->state = new_state;

            if (new_state == RESOURCE_STATE_ACTIVE)
            {
                manager->active_resources++;
                manager->pending_operations--;
            }

            return 0;
        }
    }

    return -1;
}

int cloud_resource_delete(CloudResourceManager *manager, uint32_t resource_id)
{
    if (!manager || resource_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->resource_count; i++)
    {
        if (manager->resources[i].resource_id == resource_id)
        {
            CloudResource *resource = &manager->resources[i];

            if (resource->state == RESOURCE_STATE_ACTIVE)
            {
                manager->active_resources--;
            }

            memmove(&manager->resources[i], &manager->resources[i + 1],
                    sizeof(CloudResource) * (manager->resource_count - i - 1));
            manager->resource_count--;
            return 0;
        }
    }

    return -1;
}

int cloud_resource_get_state(CloudResourceManager *manager, uint32_t resource_id,
                             ResourceState *state)
{
    if (!manager || resource_id == 0 || !state)
        return -1;

    for (uint32_t i = 0; i < manager->resource_count; i++)
    {
        if (manager->resources[i].resource_id == resource_id)
        {
            *state = manager->resources[i].state;
            return 0;
        }
    }

    return -1;
}

int cloud_resource_mark_billable(CloudResourceManager *manager, uint32_t resource_id,
                                 uint64_t cost_per_month)
{
    if (!manager || resource_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->resource_count; i++)
    {
        if (manager->resources[i].resource_id == resource_id)
        {
            manager->resources[i].is_billable = 1;
            manager->resources[i].estimated_cost_per_month = cost_per_month;
            manager->total_cost_per_month += cost_per_month;
            return 0;
        }
    }

    return -1;
}

int cloud_resource_set_cost_estimate(CloudResourceManager *manager, uint32_t resource_id,
                                     uint64_t estimated_cost_per_month)
{
    if (!manager || resource_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->resource_count; i++)
    {
        if (manager->resources[i].resource_id == resource_id)
        {
            manager->resources[i].estimated_cost_per_month = estimated_cost_per_month;
            return 0;
        }
    }

    return -1;
}

uint32_t resource_group_create(CloudResourceManager *manager, const char *group_name)
{
    if (!manager || manager->group_count >= MAX_RESOURCE_GROUPS || !group_name)
        return 0;

    ResourceGroup *group = &manager->groups[manager->group_count];
    memset(group, 0, sizeof(ResourceGroup));

    group->group_id = next_group_id++;
    strncpy(group->group_name, group_name, sizeof(group->group_name) - 1);
    group->resource_count = 0;
    group->total_cost_per_month = 0;
    group->is_auto_managed = 0;

    manager->group_count++;
    return group->group_id;
}

int resource_group_add_resource(CloudResourceManager *manager, uint32_t group_id,
                                uint32_t resource_id)
{
    if (!manager || group_id == 0 || resource_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->group_count; i++)
    {
        if (manager->groups[i].group_id == group_id)
        {
            ResourceGroup *group = &manager->groups[i];

            if (group->resource_count >= MAX_CLOUD_RESOURCES)
                return -1;

            group->resource_ids[group->resource_count++] = resource_id;
            return 0;
        }
    }

    return -1;
}

int resource_group_remove_resource(CloudResourceManager *manager, uint32_t group_id,
                                   uint32_t resource_id)
{
    if (!manager || group_id == 0 || resource_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->group_count; i++)
    {
        if (manager->groups[i].group_id == group_id)
        {
            ResourceGroup *group = &manager->groups[i];

            for (uint32_t j = 0; j < group->resource_count; j++)
            {
                if (group->resource_ids[j] == resource_id)
                {
                    memmove(&group->resource_ids[j], &group->resource_ids[j + 1],
                            sizeof(uint32_t) * (group->resource_count - j - 1));
                    group->resource_count--;
                    return 0;
                }
            }
        }
    }

    return -1;
}

int resource_group_enable_auto_management(CloudResourceManager *manager, uint32_t group_id)
{
    if (!manager || group_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->group_count; i++)
    {
        if (manager->groups[i].group_id == group_id)
        {
            manager->groups[i].is_auto_managed = 1;
            return 0;
        }
    }

    return -1;
}

uint32_t cloud_resource_find_by_tag(CloudResourceManager *manager, const char *tag_key,
                                    const char *tag_value, uint32_t *matching_resources,
                                    uint32_t max_results)
{
    if (!manager || !tag_key || !matching_resources)
        return 0;

    uint32_t count = 0;

    for (uint32_t i = 0; i < manager->resource_count && count < max_results; i++)
    {
        CloudResource *resource = &manager->resources[i];

        for (uint32_t j = 0; j < resource->tag_count; j++)
        {
            if (strncmp(resource->tags[j].tag_key, tag_key, sizeof(resource->tags[j].tag_key)) == 0)
            {
                if (!tag_value || strncmp(resource->tags[j].tag_value, tag_value,
                                          sizeof(resource->tags[j].tag_value)) == 0)
                {
                    matching_resources[count++] = resource->resource_id;
                    break;
                }
            }
        }
    }

    return count;
}

uint32_t cloud_resource_find_by_type(CloudResourceManager *manager, ResourceType type,
                                     uint32_t *matching_resources, uint32_t max_results)
{
    if (!manager || !matching_resources)
        return 0;

    uint32_t count = 0;

    for (uint32_t i = 0; i < manager->resource_count && count < max_results; i++)
    {
        if (manager->resources[i].resource_type == type)
        {
            matching_resources[count++] = manager->resources[i].resource_id;
        }
    }

    return count;
}

int cloud_resource_calculate_group_cost(CloudResourceManager *manager, uint32_t group_id,
                                        uint64_t *total_cost)
{
    if (!manager || group_id == 0 || !total_cost)
        return -1;

    *total_cost = 0;

    for (uint32_t i = 0; i < manager->group_count; i++)
    {
        if (manager->groups[i].group_id == group_id)
        {
            ResourceGroup *group = &manager->groups[i];

            for (uint32_t j = 0; j < group->resource_count; j++)
            {
                for (uint32_t k = 0; k < manager->resource_count; k++)
                {
                    if (manager->resources[k].resource_id == group->resource_ids[j])
                    {
                        *total_cost += manager->resources[k].estimated_cost_per_month;
                        break;
                    }
                }
            }

            group->total_cost_per_month = *total_cost;
            return 0;
        }
    }

    return -1;
}

int cloud_resource_manager_calculate_total_cost(CloudResourceManager *manager,
                                                uint64_t *total_cost)
{
    if (!manager || !total_cost)
        return -1;

    *total_cost = 0;

    for (uint32_t i = 0; i < manager->resource_count; i++)
    {
        if (manager->resources[i].is_billable)
        {
            *total_cost += manager->resources[i].estimated_cost_per_month;
        }
    }

    manager->total_cost_per_month = *total_cost;
    return 0;
}

int cloud_resource_manager_optimize_costs(CloudResourceManager *manager)
{
    if (!manager)
        return -1;

    for (uint32_t i = 0; i < manager->resource_count; i++)
    {
        if (manager->resources[i].state != RESOURCE_STATE_ACTIVE &&
            manager->resources[i].is_billable)
        {
            manager->total_cost_per_month -= manager->resources[i].estimated_cost_per_month;
            manager->resources[i].is_billable = 0;
        }
    }

    return 0;
}

int cloud_resource_manager_get_metrics(CloudResourceManager *manager,
                                       CloudResourceManagerMetrics *metrics)
{
    if (!manager || !metrics)
        return -1;

    metrics->manager_id = manager->manager_id;
    metrics->total_resources = manager->resource_count;
    metrics->active_resources = manager->active_resources;
    metrics->pending_operations = manager->pending_operations;
    metrics->total_groups = manager->group_count;
    metrics->total_cost_per_month = manager->total_cost_per_month;
    metrics->total_provisioned = manager->total_provisioned_resources;

    memset(metrics->resource_type_distribution, 0, sizeof(metrics->resource_type_distribution));

    for (uint32_t i = 0; i < manager->resource_count; i++)
    {
        if (manager->resources[i].resource_type < 8)
        {
            metrics->resource_type_distribution[manager->resources[i].resource_type]++;
        }
    }

    return 0;
}
