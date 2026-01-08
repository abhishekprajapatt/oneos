#ifndef GARBAGE_COLLECTOR_H
#define GARBAGE_COLLECTOR_H

#include <stdint.h>
#include <stddef.h>

#define MAX_GC_OBJECTS 65536
#define MAX_OBJECT_REFERENCES 32
#define MAX_GC_THREADS 16

typedef enum
{
    GC_STATUS_ALLOCATED,
    GC_STATUS_MARKED,
    GC_STATUS_UNMARKED,
    GC_STATUS_COLLECTED
} GCStatus;

typedef enum
{
    GC_ALGORITHM_MARK_SWEEP,
    GC_ALGORITHM_COPY,
    GC_ALGORITHM_GENERATIONAL,
    GC_ALGORITHM_CONCURRENT,
    GC_ALGORITHM_INCREMENTAL
} GCAlgorithm;

typedef struct
{
    uint32_t object_id;
    void *object_ptr;
    uint64_t object_size;
    GCStatus status;

    uint32_t reference_count;
    uint32_t referenced_objects[MAX_OBJECT_REFERENCES];

    uint64_t allocation_time;
    uint64_t last_access_time;
    uint32_t access_count;
} GCObject;

typedef struct
{
    uint32_t gc_id;
    GCAlgorithm algorithm;

    uint32_t object_count;
    GCObject objects[MAX_GC_OBJECTS];

    uint64_t total_collected;
    uint64_t total_objects_collected;

    uint64_t gc_start_time;
    uint64_t gc_end_time;
    uint64_t last_gc_duration_ms;

    uint32_t gc_cycle_count;
    uint32_t major_gc_count;
    uint32_t minor_gc_count;

    float gc_efficiency;
    float pause_time_ms;

    uint8_t is_concurrent;
    uint8_t is_generational;
} GarbageCollector;

GarbageCollector *gc_init(GCAlgorithm algorithm);
int gc_destroy(GarbageCollector *gc);

uint32_t gc_allocate_object(GarbageCollector *gc, uint64_t object_size);
int gc_deallocate_object(GarbageCollector *gc, uint32_t object_id);

int gc_add_reference(GarbageCollector *gc, uint32_t from_object, uint32_t to_object);
int gc_remove_reference(GarbageCollector *gc, uint32_t from_object, uint32_t to_object);

int gc_mark_phase(GarbageCollector *gc);
int gc_sweep_phase(GarbageCollector *gc);
int gc_run_full_collection(GarbageCollector *gc);
int gc_run_incremental_collection(GarbageCollector *gc);

int gc_mark_object(GarbageCollector *gc, uint32_t object_id);
int gc_unmark_object(GarbageCollector *gc, uint32_t object_id);
int gc_collect_object(GarbageCollector *gc, uint32_t object_id);

int gc_enable_concurrent_mode(GarbageCollector *gc);
int gc_disable_concurrent_mode(GarbageCollector *gc);

int gc_set_heap_size(GarbageCollector *gc, uint64_t heap_size);
int gc_get_heap_usage(GarbageCollector *gc);

typedef struct
{
    uint32_t gc_id;
    uint64_t total_collected;
    uint64_t total_objects_collected;
    uint64_t gc_cycles;
    uint64_t major_collections;
    uint64_t minor_collections;
    float average_pause_time_ms;
    float collection_efficiency;
} GCMetrics;

int gc_get_metrics(GarbageCollector *gc, GCMetrics *metrics);

#endif
