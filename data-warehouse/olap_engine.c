#include "olap_engine.h"
#include <stdlib.h>
#include <string.h>

OLAPEngine *olap_init(void)
{
    OLAPEngine *engine = (OLAPEngine *)malloc(sizeof(OLAPEngine));
    if (!engine)
        return NULL;

    memset(engine, 0, sizeof(OLAPEngine));
    engine->cube_count = 0;
    engine->total_cells = 0;

    return engine;
}

void olap_cleanup(OLAPEngine *engine)
{
    if (!engine)
        return;

    for (uint32_t i = 0; i < engine->cube_count; i++)
    {
        for (uint32_t j = 0; j < engine->cubes[i].dimension_count; j++)
        {
            for (uint32_t k = 0; k < engine->cubes[i].dimensions[j].hierarchy_count; k++)
            {
                for (uint32_t l = 0; l < engine->cubes[i].dimensions[j].hierarchies[k].level_count; l++)
                {
                    if (engine->cubes[i].dimensions[j].hierarchies[k].levels[l].members)
                        free(engine->cubes[i].dimensions[j].hierarchies[k].levels[l].members);
                }
            }
        }
    }

    memset(engine, 0, sizeof(OLAPEngine));
    free(engine);
}

uint32_t olap_create_cube(OLAPEngine *engine, const char *name, uint32_t warehouse_id, uint32_t schema_id)
{
    if (!engine || !name || engine->cube_count >= MAX_CUBES)
        return 0;

    OLAPCube *cube = &engine->cubes[engine->cube_count];
    cube->cube_id = engine->cube_count + 1;
    strncpy(cube->cube_name, name, 127);
    cube->warehouse_id = warehouse_id;
    cube->schema_id = schema_id;
    cube->dimension_count = 0;
    cube->measure_count = 0;
    cube->cell_count = 0;
    cube->created_at = time(NULL);
    cube->last_refresh = cube->created_at;
    cube->materialized = false;

    engine->cube_count++;
    return cube->cube_id;
}

int olap_delete_cube(OLAPEngine *engine, uint32_t cube_id)
{
    if (!engine || cube_id == 0 || cube_id > engine->cube_count)
        return -1;

    OLAPCube *cube = &engine->cubes[cube_id - 1];
    memset(cube, 0, sizeof(OLAPCube));

    return 0;
}

uint32_t olap_add_dimension(OLAPEngine *engine, uint32_t cube_id, const char *dim_name)
{
    if (!engine || cube_id == 0 || cube_id > engine->cube_count || !dim_name)
        return 0;

    OLAPCube *cube = &engine->cubes[cube_id - 1];

    if (cube->dimension_count >= MAX_DIMENSIONS_PER_CUBE)
        return 0;

    Dimension *dimension = &cube->dimensions[cube->dimension_count];
    dimension->dimension_id = cube->dimension_count + 1;
    strncpy(dimension->dimension_name, dim_name, 127);
    dimension->hierarchy_count = 0;
    dimension->cardinality = 0;
    dimension->has_time = false;

    cube->dimension_count++;
    return dimension->dimension_id;
}

int olap_add_hierarchy_to_dimension(OLAPEngine *engine, uint32_t cube_id, uint32_t dimension_id, const char *hierarchy_name)
{
    if (!engine || cube_id == 0 || cube_id > engine->cube_count || dimension_id == 0 || !hierarchy_name)
        return -1;

    OLAPCube *cube = &engine->cubes[cube_id - 1];

    if (dimension_id > cube->dimension_count)
        return -2;

    Dimension *dimension = &cube->dimensions[dimension_id - 1];

    if (dimension->hierarchy_count >= MAX_HIERARCHY_LEVELS)
        return -3;

    Hierarchy *hierarchy = &dimension->hierarchies[dimension->hierarchy_count];
    hierarchy->hierarchy_id = dimension->hierarchy_count + 1;
    strncpy(hierarchy->hierarchy_name, hierarchy_name, 127);
    hierarchy->dimension_id = dimension_id;
    hierarchy->level_count = 0;

    dimension->hierarchy_count++;
    return 0;
}

uint32_t olap_add_measure(OLAPEngine *engine, uint32_t cube_id, const char *measure_name, AggregationType agg_type)
{
    if (!engine || cube_id == 0 || cube_id > engine->cube_count || !measure_name)
        return 0;

    OLAPCube *cube = &engine->cubes[cube_id - 1];

    if (cube->measure_count >= MAX_MEASURES_PER_CUBE)
        return 0;

    Measure *measure = &cube->measures[cube->measure_count];
    measure->measure_id = cube->measure_count + 1;
    strncpy(measure->measure_name, measure_name, 127);
    measure->agg_type = agg_type;
    measure->precision = 2;
    measure->current_value = 0.0;
    measure->sum_value = 0.0;
    measure->min_value = 0.0;
    measure->max_value = 0.0;
    measure->count = 0;

    cube->measure_count++;
    return measure->measure_id;
}

int olap_update_measure_value(OLAPEngine *engine, uint32_t cube_id, uint32_t measure_id, double value)
{
    if (!engine || cube_id == 0 || cube_id > engine->cube_count || measure_id == 0)
        return -1;

    OLAPCube *cube = &engine->cubes[cube_id - 1];

    if (measure_id > cube->measure_count)
        return -2;

    Measure *measure = &cube->measures[measure_id - 1];
    measure->current_value = value;
    measure->sum_value += value;

    if (measure->count == 0)
    {
        measure->min_value = value;
        measure->max_value = value;
    }
    else
    {
        if (value < measure->min_value)
            measure->min_value = value;
        if (value > measure->max_value)
            measure->max_value = value;
    }

    measure->count++;

    return 0;
}

int olap_perform_operation(OLAPEngine *engine, uint32_t cube_id, OLAPOperation op, const char *dim_path)
{
    if (!engine || cube_id == 0 || cube_id > engine->cube_count || !dim_path)
        return -1;

    OLAPCube *cube = &engine->cubes[cube_id - 1];

    switch (op)
    {
    case DRILL_DOWN:
        cube->cell_count *= 2;
        break;
    case DRILL_UP:
        cube->cell_count /= 2;
        break;
    case SLICE:
    case DICE:
        cube->cell_count = (cube->cell_count * 75) / 100;
        break;
    case PIVOT:
        break;
    case DRILL_THROUGH:
        cube->cell_count += (cube->cell_count / 10);
        break;
    }

    cube->last_refresh = time(NULL);
    return 0;
}

int olap_slice_cube(OLAPEngine *engine, uint32_t cube_id, uint32_t dimension_id, const char *member, double **result, uint32_t *result_size)
{
    if (!engine || cube_id == 0 || cube_id > engine->cube_count || dimension_id == 0 || !member || !result || !result_size)
        return -1;

    OLAPCube *cube = &engine->cubes[cube_id - 1];

    if (dimension_id > cube->dimension_count)
        return -2;

    *result_size = cube->measure_count;
    *result = (double *)malloc(sizeof(double) * (*result_size));

    for (uint32_t i = 0; i < cube->measure_count; i++)
    {
        (*result)[i] = cube->measures[i].current_value;
    }

    return 0;
}

int olap_dice_cube(OLAPEngine *engine, uint32_t cube_id, uint32_t *dimension_ids, const char **members, uint32_t filter_count)
{
    if (!engine || cube_id == 0 || cube_id > engine->cube_count || !dimension_ids || !members || filter_count == 0)
        return -1;

    OLAPCube *cube = &engine->cubes[cube_id - 1];

    for (uint32_t i = 0; i < filter_count; i++)
    {
        if (dimension_ids[i] > cube->dimension_count)
            return -2;
    }

    cube->cell_count = (cube->cell_count / (filter_count + 1));
    cube->last_refresh = time(NULL);

    return 0;
}

int olap_materialize_cube(OLAPEngine *engine, uint32_t cube_id)
{
    if (!engine || cube_id == 0 || cube_id > engine->cube_count)
        return -1;

    OLAPCube *cube = &engine->cubes[cube_id - 1];

    uint64_t cell_count = 1;
    for (uint32_t i = 0; i < cube->dimension_count; i++)
    {
        cell_count *= (cube->dimensions[i].cardinality > 0) ? cube->dimensions[i].cardinality : 100;
    }

    cube->cell_count = cell_count;
    cube->materialized = true;
    cube->last_refresh = time(NULL);
    engine->total_cells += cell_count;

    return 0;
}

int olap_get_cell(OLAPEngine *engine, uint32_t cube_id, const char *coordinates, double *value)
{
    if (!engine || cube_id == 0 || cube_id > engine->cube_count || !coordinates || !value)
        return -1;

    OLAPCube *cube = &engine->cubes[cube_id - 1];

    if (cube->measure_count == 0)
        return -2;

    *value = cube->measures[0].current_value;

    return 0;
}
