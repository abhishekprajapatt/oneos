#ifndef SPREADSHEET_H
#define SPREADSHEET_H

#include <stdint.h>

typedef struct
{
    uint32_t row;
    uint32_t column;
    const char *cell_value;
    const char *cell_format;
} cell_data_t;

typedef struct
{
    uint32_t total_rows;
    uint32_t total_columns;
    uint32_t active_sheet;
} spreadsheet_state_t;

int spreadsheet_init(void);
int spreadsheet_create(const char *title);
int spreadsheet_open(const char *file_path);
int spreadsheet_save(void);
int spreadsheet_set_cell(uint32_t row, uint32_t column, const char *value);
int spreadsheet_get_cell(uint32_t row, uint32_t column, char *value, uint32_t max_len);
int spreadsheet_insert_formula(uint32_t row, uint32_t column, const char *formula);
int spreadsheet_insert_row(uint32_t row_number);
int spreadsheet_delete_row(uint32_t row_number);
int spreadsheet_export(const char *format);

#endif
