#ifndef OLAP_ENGINE_H
#define OLAP_ENGINE_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#define MAX_CUBES 128
#define MAX_DIMENSIONS_PER_CUBE 64
#define MAX_MEASURES_PER_CUBE 256
#define MAX_HIERARCHIES 512
#define MAX_HIERARCHY_LEVELS 32
#define MAX_AGGREGATIONS 1024

typedef enum
{
    AGGREGATION_SUM,
    AGGREGATION_COUNT,
    AGGREGATION_AVG,
    AGGREGATION_MIN,
    AGGREGATION_MAX,
    AGGREGATION_STDDEV,
    AGGREGATION_VARIANCE,
    AGGREGATION_PERCENTILE
} AggregationType;

typedef enum
{
    DRILL_DOWN,
    DRILL_UP,
    DRILL_THROUGH,
    SLICE,
    DICE,
    PIVOT
} OLAPOperation;

typedef struct
{
    uint32_t level_id;
    char level_name[128];
    uint32_t level_depth;
    uint32_t member_count;
    char **members;
} HierarchyLevel;

typedef struct
{
    uint32_t hierarchy_id;
    char hierarchy_name[128];
    uint32_t dimension_id;
    uint32_t level_count;
    HierarchyLevel levels[MAX_HIERARCHY_LEVELS];
} Hierarchy;

typedef struct
{
    uint32_t dimension_id;
    char dimension_name[128];
    uint32_t hierarchy_count;
    Hierarchy hierarchies[MAX_HIERARCHY_LEVELS];
    uint32_t cardinality;
    bool has_time;
} Dimension;

typedef struct
{
    uint32_t measure_id;
    char measure_name[128];
    AggregationType agg_type;
    uint32_t precision;
    double current_value;
    double sum_value;
    double min_value;
    double max_value;
    uint64_t count;
} Measure;

typedef struct
{
    uint32_t cube_id;
    char cube_name[128];
    uint32_t warehouse_id;
    uint32_t schema_id;
    Dimension dimensions[MAX_DIMENSIONS_PER_CUBE];
    uint32_t dimension_count;
    Measure measures[MAX_MEASURES_PER_CUBE];
    uint32_t measure_count;
    uint64_t cell_count;
    time_t created_at;
    time_t last_refresh;
    bool materialized;
} OLAPCube;

typedef struct
{
    OLAPCube cubes[MAX_CUBES];
    uint32_t cube_count;
    uint64_t total_cells;
} OLAPEngine;

OLAPEngine *olap_init(void);
void olap_cleanup(OLAPEngine *engine);

uint32_t olap_create_cube(OLAPEngine *engine, const char *name, uint32_t warehouse_id, uint32_t schema_id);
int olap_delete_cube(OLAPEngine *engine, uint32_t cube_id);

uint32_t olap_add_dimension(OLAPEngine *engine, uint32_t cube_id, const char *dim_name);
int olap_add_hierarchy_to_dimension(OLAPEngine *engine, uint32_t cube_id, uint32_t dimension_id, const char *hierarchy_name);

uint32_t olap_add_measure(OLAPEngine *engine, uint32_t cube_id, const char *measure_name, AggregationType agg_type);
int olap_update_measure_value(OLAPEngine *engine, uint32_t cube_id, uint32_t measure_id, double value);

int olap_perform_operation(OLAPEngine *engine, uint32_t cube_id, OLAPOperation op, const char *dim_path);
int olap_slice_cube(OLAPEngine *engine, uint32_t cube_id, uint32_t dimension_id, const char *member, double **result, uint32_t *result_size);
int olap_dice_cube(OLAPEngine *engine, uint32_t cube_id, uint32_t *dimension_ids, const char **members, uint32_t filter_count);

int olap_materialize_cube(OLAPEngine *engine, uint32_t cube_id);
int olap_get_cell(OLAPEngine *engine, uint32_t cube_id, const char *coordinates, double *value);

#endif
