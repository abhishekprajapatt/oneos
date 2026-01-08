#include "garbage_collector.h"
#include <stdlib.h>
#include <string.h>

static GarbageCollector *global_gc = NULL;
static uint32_t next_object_id = 1;

GarbageCollector *gc_init(GCAlgorithm algorithm)
{
    GarbageCollector *gc = (GarbageCollector *)malloc(sizeof(GarbageCollector));
    if (!gc)
        return NULL;

    memset(gc, 0, sizeof(GarbageCollector));
    gc->gc_id = 1;
    gc->algorithm = algorithm;
    gc->object_count = 0;
    gc->total_collected = 0;
    gc->total_objects_collected = 0;
    gc->gc_cycle_count = 0;
    gc->major_gc_count = 0;
    gc->minor_gc_count = 0;
    gc->gc_efficiency = 0.0f;
    gc->pause_time_ms = 0.0f;
    gc->is_concurrent = (algorithm == GC_ALGORITHM_CONCURRENT) ? 1 : 0;
    gc->is_generational = (algorithm == GC_ALGORITHM_GENERATIONAL) ? 1 : 0;

    global_gc = gc;
    return gc;
}

int gc_destroy(GarbageCollector *gc)
{
    if (!gc)
        return -1;

    free(gc);
    global_gc = NULL;
    return 0;
}

uint32_t gc_allocate_object(GarbageCollector *gc, uint64_t object_size)
{
    if (!gc || gc->object_count >= MAX_GC_OBJECTS || object_size == 0)
        return 0;

    GCObject *obj = &gc->objects[gc->object_count];
    memset(obj, 0, sizeof(GCObject));

    obj->object_id = next_object_id++;
    obj->object_ptr = malloc(object_size);

    if (!obj->object_ptr)
        return 0;

    obj->object_size = object_size;
    obj->status = GC_STATUS_ALLOCATED;
    obj->reference_count = 0;
    obj->allocation_time = 0;
    obj->last_access_time = 0;
    obj->access_count = 0;

    gc->object_count++;
    return obj->object_id;
}

int gc_deallocate_object(GarbageCollector *gc, uint32_t object_id)
{
    if (!gc)
        return -1;

    for (uint32_t i = 0; i < gc->object_count; i++)
    {
        if (gc->objects[i].object_id == object_id)
        {
            free(gc->objects[i].object_ptr);
            memset(&gc->objects[i], 0, sizeof(GCObject));
            return 0;
        }
    }

    return -1;
}

int gc_add_reference(GarbageCollector *gc, uint32_t from_object, uint32_t to_object)
{
    if (!gc)
        return -1;

    for (uint32_t i = 0; i < gc->object_count; i++)
    {
        if (gc->objects[i].object_id == from_object)
        {
            if (gc->objects[i].reference_count >= MAX_OBJECT_REFERENCES)
                return -1;

            gc->objects[i].referenced_objects[gc->objects[i].reference_count++] = to_object;
            return 0;
        }
    }

    return -1;
}

int gc_remove_reference(GarbageCollector *gc, uint32_t from_object, uint32_t to_object)
{
    if (!gc)
        return -1;

    for (uint32_t i = 0; i < gc->object_count; i++)
    {
        if (gc->objects[i].object_id == from_object)
        {
            for (uint32_t j = 0; j < gc->objects[i].reference_count; j++)
            {
                if (gc->objects[i].referenced_objects[j] == to_object)
                {
                    gc->objects[i].referenced_objects[j] = 0;
                    gc->objects[i].reference_count--;
                    return 0;
                }
            }
        }
    }

    return -1;
}

int gc_mark_phase(GarbageCollector *gc)
{
    if (!gc)
        return -1;

    for (uint32_t i = 0; i < gc->object_count; i++)
    {
        if (gc->objects[i].object_ptr != NULL && gc->objects[i].reference_count > 0)
        {
            gc->objects[i].status = GC_STATUS_MARKED;
        }
        else
        {
            gc->objects[i].status = GC_STATUS_UNMARKED;
        }
    }

    return 0;
}

int gc_sweep_phase(GarbageCollector *gc)
{
    if (!gc)
        return -1;

    uint32_t collected = 0;

    for (uint32_t i = 0; i < gc->object_count; i++)
    {
        if (gc->objects[i].status == GC_STATUS_UNMARKED && gc->objects[i].object_ptr != NULL)
        {
            gc->total_collected += gc->objects[i].object_size;
            free(gc->objects[i].object_ptr);
            memset(&gc->objects[i], 0, sizeof(GCObject));
            collected++;
        }
    }

    gc->total_objects_collected += collected;
    return collected;
}

int gc_run_full_collection(GarbageCollector *gc)
{
    if (!gc)
        return -1;

    gc->gc_start_time = 0;

    gc_mark_phase(gc);
    int swept = gc_sweep_phase(gc);

    gc->gc_end_time = 0;
    gc->last_gc_duration_ms = 0;
    gc->gc_cycle_count++;
    gc->major_gc_count++;

    return swept;
}

int gc_run_incremental_collection(GarbageCollector *gc)
{
    if (!gc)
        return -1;

    uint32_t batch_size = gc->object_count / 10;
    if (batch_size == 0)
        batch_size = 1;

    for (uint32_t i = 0; i < batch_size && i < gc->object_count; i++)
    {
        if (gc->objects[i].reference_count == 0 && gc->objects[i].object_ptr != NULL)
        {
            gc_collect_object(gc, gc->objects[i].object_id);
        }
    }

    gc->minor_gc_count++;
    return 0;
}

int gc_mark_object(GarbageCollector *gc, uint32_t object_id)
{
    if (!gc)
        return -1;

    for (uint32_t i = 0; i < gc->object_count; i++)
    {
        if (gc->objects[i].object_id == object_id)
        {
            gc->objects[i].status = GC_STATUS_MARKED;
            return 0;
        }
    }

    return -1;
}

int gc_unmark_object(GarbageCollector *gc, uint32_t object_id)
{
    if (!gc)
        return -1;

    for (uint32_t i = 0; i < gc->object_count; i++)
    {
        if (gc->objects[i].object_id == object_id)
        {
            gc->objects[i].status = GC_STATUS_UNMARKED;
            return 0;
        }
    }

    return -1;
}

int gc_collect_object(GarbageCollector *gc, uint32_t object_id)
{
    if (!gc)
        return -1;

    for (uint32_t i = 0; i < gc->object_count; i++)
    {
        if (gc->objects[i].object_id == object_id)
        {
            gc->total_collected += gc->objects[i].object_size;
            free(gc->objects[i].object_ptr);
            memset(&gc->objects[i], 0, sizeof(GCObject));
            gc->total_objects_collected++;
            return 0;
        }
    }

    return -1;
}

int gc_enable_concurrent_mode(GarbageCollector *gc)
{
    if (!gc)
        return -1;

    gc->is_concurrent = 1;
    return 0;
}

int gc_disable_concurrent_mode(GarbageCollector *gc)
{
    if (!gc)
        return -1;

    gc->is_concurrent = 0;
    return 0;
}

int gc_set_heap_size(GarbageCollector *gc, uint64_t heap_size)
{
    if (!gc || heap_size == 0)
        return -1;

    return 0;
}

int gc_get_heap_usage(GarbageCollector *gc)
{
    if (!gc)
        return -1;

    uint64_t total_size = 0;
    for (uint32_t i = 0; i < gc->object_count; i++)
    {
        if (gc->objects[i].object_ptr != NULL)
        {
            total_size += gc->objects[i].object_size;
        }
    }

    return total_size;
}

int gc_get_metrics(GarbageCollector *gc, GCMetrics *metrics)
{
    if (!gc || !metrics)
        return -1;

    metrics->gc_id = gc->gc_id;
    metrics->total_collected = gc->total_collected;
    metrics->total_objects_collected = gc->total_objects_collected;
    metrics->gc_cycles = gc->gc_cycle_count;
    metrics->major_collections = gc->major_gc_count;
    metrics->minor_collections = gc->minor_gc_count;
    metrics->average_pause_time_ms = gc->pause_time_ms;
    metrics->collection_efficiency = gc->gc_efficiency;

    return 0;
}
