#include "warehouse_core.h"
#include <stdlib.h>
#include <string.h>

WarehouseInfrastructure *warehouse_init(void)
{
    WarehouseInfrastructure *wh = (WarehouseInfrastructure *)malloc(sizeof(WarehouseInfrastructure));
    if (!wh)
        return NULL;

    memset(wh, 0, sizeof(WarehouseInfrastructure));
    wh->warehouse_count = 0;

    return wh;
}

void warehouse_cleanup(WarehouseInfrastructure *wh)
{
    if (!wh)
        return;
    memset(wh, 0, sizeof(WarehouseInfrastructure));
    free(wh);
}

uint32_t warehouse_create(WarehouseInfrastructure *wh, const char *name)
{
    if (!wh || !name || wh->warehouse_count >= MAX_WAREHOUSES)
        return 0;

    DataWarehouse *warehouse = &wh->warehouses[wh->warehouse_count];
    warehouse->warehouse_id = wh->warehouse_count + 1;
    strncpy(warehouse->warehouse_name, name, 127);
    warehouse->schema_count = 0;
    warehouse->total_size_bytes = 0;
    warehouse->total_rows = 0;
    warehouse->created_at = time(NULL);
    warehouse->active = true;

    wh->warehouse_count++;
    return warehouse->warehouse_id;
}

int warehouse_delete(WarehouseInfrastructure *wh, uint32_t warehouse_id)
{
    if (!wh || warehouse_id == 0 || warehouse_id > wh->warehouse_count)
        return -1;

    DataWarehouse *warehouse = &wh->warehouses[warehouse_id - 1];
    warehouse->active = false;

    return 0;
}

uint32_t warehouse_create_schema(WarehouseInfrastructure *wh, uint32_t warehouse_id, const char *name, SchemaType type)
{
    if (!wh || warehouse_id == 0 || warehouse_id > wh->warehouse_count || !name)
        return 0;

    DataWarehouse *warehouse = &wh->warehouses[warehouse_id - 1];

    if (warehouse->schema_count >= MAX_SCHEMAS)
        return 0;

    Schema *schema = &warehouse->schemas[warehouse->schema_count];
    schema->schema_id = warehouse->schema_count + 1;
    strncpy(schema->schema_name, name, 127);
    schema->type = type;
    schema->table_count = 0;
    schema->created_at = time(NULL);
    schema->is_active = true;

    warehouse->schema_count++;
    return schema->schema_id;
}

int warehouse_get_schema(WarehouseInfrastructure *wh, uint32_t warehouse_id, uint32_t schema_id, Schema **schema)
{
    if (!wh || warehouse_id == 0 || warehouse_id > wh->warehouse_count || schema_id == 0 || !schema)
        return -1;

    DataWarehouse *warehouse = &wh->warehouses[warehouse_id - 1];

    if (schema_id > warehouse->schema_count)
        return -2;

    *schema = &warehouse->schemas[schema_id - 1];

    return 0;
}

uint32_t warehouse_create_table(WarehouseInfrastructure *wh, uint32_t warehouse_id, uint32_t schema_id, const char *name, TableType type)
{
    if (!wh || warehouse_id == 0 || warehouse_id > wh->warehouse_count || schema_id == 0 || !name)
        return 0;

    DataWarehouse *warehouse = &wh->warehouses[warehouse_id - 1];

    if (schema_id > warehouse->schema_count)
        return 0;

    Schema *schema = &warehouse->schemas[schema_id - 1];

    if (schema->table_count >= MAX_TABLES)
        return 0;

    Table *table = &schema->tables[schema->table_count];
    table->table_id = schema->table_count + 1;
    strncpy(table->table_name, name, 127);
    table->type = type;
    table->schema_id = schema_id;
    table->column_count = 0;
    table->row_count = 0;
    table->size_bytes = 0;
    table->created_at = time(NULL);
    table->last_modified = table->created_at;
    table->compressed = false;

    schema->table_count++;
    return table->table_id;
}

int warehouse_add_column(WarehouseInfrastructure *wh, uint32_t warehouse_id, uint32_t schema_id, uint32_t table_id, const char *col_name, ColumnType col_type)
{
    if (!wh || warehouse_id == 0 || warehouse_id > wh->warehouse_count || schema_id == 0 || table_id == 0 || !col_name)
        return -1;

    DataWarehouse *warehouse = &wh->warehouses[warehouse_id - 1];

    if (schema_id > warehouse->schema_count)
        return -2;

    Schema *schema = &warehouse->schemas[schema_id - 1];

    if (table_id > schema->table_count)
        return -3;

    Table *table = &schema->tables[table_id - 1];

    if (table->column_count >= MAX_COLUMNS)
        return -4;

    Column *column = &table->columns[table->column_count];
    column->column_id = table->column_count + 1;
    strncpy(column->column_name, col_name, 127);
    column->type = col_type;
    column->size = 0;
    column->is_null = true;
    column->is_primary_key = false;
    column->is_indexed = false;

    table->column_count++;
    return 0;
}

int warehouse_get_table_stats(WarehouseInfrastructure *wh, uint32_t warehouse_id, uint32_t schema_id, uint32_t table_id, uint64_t *rows, uint64_t *bytes)
{
    if (!wh || warehouse_id == 0 || warehouse_id > wh->warehouse_count || schema_id == 0 || table_id == 0 || !rows || !bytes)
        return -1;

    DataWarehouse *warehouse = &wh->warehouses[warehouse_id - 1];

    if (schema_id > warehouse->schema_count)
        return -2;

    Schema *schema = &warehouse->schemas[schema_id - 1];

    if (table_id > schema->table_count)
        return -3;

    Table *table = &schema->tables[table_id - 1];
    *rows = table->row_count;
    *bytes = table->size_bytes;

    return 0;
}

int warehouse_insert_row(WarehouseInfrastructure *wh, uint32_t warehouse_id, uint32_t schema_id, uint32_t table_id, void *row_data)
{
    if (!wh || warehouse_id == 0 || warehouse_id > wh->warehouse_count || schema_id == 0 || table_id == 0 || !row_data)
        return -1;

    DataWarehouse *warehouse = &wh->warehouses[warehouse_id - 1];

    if (schema_id > warehouse->schema_count)
        return -2;

    Schema *schema = &warehouse->schemas[schema_id - 1];

    if (table_id > schema->table_count)
        return -3;

    Table *table = &schema->tables[table_id - 1];

    if (table->row_count >= MAX_ROWS)
        return -4;

    table->row_count++;
    table->last_modified = time(NULL);
    warehouse->total_rows++;

    return 0;
}

int warehouse_update_row(WarehouseInfrastructure *wh, uint32_t warehouse_id, uint32_t schema_id, uint32_t table_id, uint64_t row_id, void *row_data)
{
    if (!wh || warehouse_id == 0 || warehouse_id > wh->warehouse_count || schema_id == 0 || table_id == 0 || !row_data)
        return -1;

    DataWarehouse *warehouse = &wh->warehouses[warehouse_id - 1];

    if (schema_id > warehouse->schema_count)
        return -2;

    Schema *schema = &warehouse->schemas[schema_id - 1];

    if (table_id > schema->table_count)
        return -3;

    Table *table = &schema->tables[table_id - 1];

    if (row_id == 0 || row_id > table->row_count)
        return -4;

    table->last_modified = time(NULL);

    return 0;
}

int warehouse_delete_row(WarehouseInfrastructure *wh, uint32_t warehouse_id, uint32_t schema_id, uint32_t table_id, uint64_t row_id)
{
    if (!wh || warehouse_id == 0 || warehouse_id > wh->warehouse_count || schema_id == 0 || table_id == 0)
        return -1;

    DataWarehouse *warehouse = &wh->warehouses[warehouse_id - 1];

    if (schema_id > warehouse->schema_count)
        return -2;

    Schema *schema = &warehouse->schemas[schema_id - 1];

    if (table_id > schema->table_count)
        return -3;

    Table *table = &schema->tables[table_id - 1];

    if (row_id == 0 || row_id > table->row_count)
        return -4;

    table->row_count--;
    table->last_modified = time(NULL);
    warehouse->total_rows--;

    return 0;
}

int warehouse_create_index(WarehouseInfrastructure *wh, uint32_t warehouse_id, uint32_t schema_id, uint32_t table_id, uint32_t column_id)
{
    if (!wh || warehouse_id == 0 || warehouse_id > wh->warehouse_count || schema_id == 0 || table_id == 0 || column_id == 0)
        return -1;

    DataWarehouse *warehouse = &wh->warehouses[warehouse_id - 1];

    if (schema_id > warehouse->schema_count)
        return -2;

    Schema *schema = &warehouse->schemas[schema_id - 1];

    if (table_id > schema->table_count)
        return -3;

    Table *table = &schema->tables[table_id - 1];

    if (column_id > table->column_count)
        return -4;

    table->columns[column_id - 1].is_indexed = true;

    return 0;
}

int warehouse_compress_table(WarehouseInfrastructure *wh, uint32_t warehouse_id, uint32_t schema_id, uint32_t table_id)
{
    if (!wh || warehouse_id == 0 || warehouse_id > wh->warehouse_count || schema_id == 0 || table_id == 0)
        return -1;

    DataWarehouse *warehouse = &wh->warehouses[warehouse_id - 1];

    if (schema_id > warehouse->schema_count)
        return -2;

    Schema *schema = &warehouse->schemas[schema_id - 1];

    if (table_id > schema->table_count)
        return -3;

    Table *table = &schema->tables[table_id - 1];
    table->compressed = true;
    table->last_modified = time(NULL);

    return 0;
}
