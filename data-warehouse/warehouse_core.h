#ifndef WAREHOUSE_CORE_H
#define WAREHOUSE_CORE_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#define MAX_WAREHOUSES 16
#define MAX_SCHEMAS 32

typedef enum
{
    SCHEMA_TYPE_STAR,
    SCHEMA_TYPE_SNOWFLAKE,
    SCHEMA_TYPE_GALAXY,
    SCHEMA_TYPE_NORMALIZED,
    SCHEMA_TYPE_DENORMALIZED
} SchemaType;

typedef enum
{
    TABLE_TYPE_FACT,
    TABLE_TYPE_DIMENSION,
    TABLE_TYPE_BRIDGE,
    TABLE_TYPE_STAGING,
    TABLE_TYPE_AGGREGATE
} TableType;

typedef enum
{
    COLUMN_TYPE_INT,
    COLUMN_TYPE_FLOAT,
    COLUMN_TYPE_STRING,
    COLUMN_TYPE_DATE,
    COLUMN_TYPE_TIMESTAMP,
    COLUMN_TYPE_BOOLEAN,
    COLUMN_TYPE_DECIMAL
} ColumnType;

typedef struct
{
    uint32_t column_id;
    char column_name[128];
    ColumnType type;
    uint32_t size;
    bool is_null;
    bool is_primary_key;
    bool is_indexed;
} Column;

typedef struct
{
    uint32_t table_id;
    char table_name[128];
    TableType type;
    uint32_t schema_id;
    uint32_t column_count;
    Column columns[MAX_COLUMNS];
    uint64_t row_count;
    uint64_t size_bytes;
    time_t created_at;
    time_t last_modified;
    bool compressed;
} Table;

typedef struct
{
    uint32_t schema_id;
    char schema_name[128];
    SchemaType type;
    uint32_t table_count;
    Table tables[MAX_TABLES];
    time_t created_at;
    bool is_active;
} Schema;

typedef struct
{
    uint32_t warehouse_id;
    char warehouse_name[128];
    Schema schemas[MAX_SCHEMAS];
    uint32_t schema_count;
    uint64_t total_size_bytes;
    uint64_t total_rows;
    time_t created_at;
    bool active;
} DataWarehouse;

typedef struct
{
    DataWarehouse warehouses[MAX_WAREHOUSES];
    uint32_t warehouse_count;
} WarehouseInfrastructure;

WarehouseInfrastructure *warehouse_init(void);
void warehouse_cleanup(WarehouseInfrastructure *wh);

uint32_t warehouse_create(WarehouseInfrastructure *wh, const char *name);
int warehouse_delete(WarehouseInfrastructure *wh, uint32_t warehouse_id);

uint32_t warehouse_create_schema(WarehouseInfrastructure *wh, uint32_t warehouse_id, const char *name, SchemaType type);
int warehouse_get_schema(WarehouseInfrastructure *wh, uint32_t warehouse_id, uint32_t schema_id, Schema **schema);

uint32_t warehouse_create_table(WarehouseInfrastructure *wh, uint32_t warehouse_id, uint32_t schema_id, const char *name, TableType type);
int warehouse_add_column(WarehouseInfrastructure *wh, uint32_t warehouse_id, uint32_t schema_id, uint32_t table_id, const char *col_name, ColumnType col_type);
int warehouse_get_table_stats(WarehouseInfrastructure *wh, uint32_t warehouse_id, uint32_t schema_id, uint32_t table_id, uint64_t *rows, uint64_t *bytes);

int warehouse_insert_row(WarehouseInfrastructure *wh, uint32_t warehouse_id, uint32_t schema_id, uint32_t table_id, void *row_data);
int warehouse_update_row(WarehouseInfrastructure *wh, uint32_t warehouse_id, uint32_t schema_id, uint32_t table_id, uint64_t row_id, void *row_data);
int warehouse_delete_row(WarehouseInfrastructure *wh, uint32_t warehouse_id, uint32_t schema_id, uint32_t table_id, uint64_t row_id);

int warehouse_create_index(WarehouseInfrastructure *wh, uint32_t warehouse_id, uint32_t schema_id, uint32_t table_id, uint32_t column_id);
int warehouse_compress_table(WarehouseInfrastructure *wh, uint32_t warehouse_id, uint32_t schema_id, uint32_t table_id);

#endif
