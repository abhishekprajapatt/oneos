#ifndef TEXMAKER_H
#define TEXMAKER_H

#include <stdint.h>

typedef struct
{
    const char *document_path;
    const char *output_format;
    uint32_t auto_save;
    uint32_t syntax_highlighting;
} texmaker_editor_options_t;

typedef struct
{
    uint32_t total_characters;
    uint32_t total_lines;
    uint32_t unsaved_changes;
} texmaker_document_info_t;

int texmaker_init(void);
int texmaker_open_document(const char *document_path);
int texmaker_create_new_document(void);
int texmaker_compile_document(const char *document_path);
int texmaker_get_document_info(texmaker_document_info_t *info);
int texmaker_view_pdf(const char *pdf_path);

#endif
