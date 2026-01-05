#ifndef DOC_H
#define DOC_H

#include <stdint.h>

typedef struct
{
    const char *doc_name;
    const char *format;
    const char *author;
    uint32_t version;
    uint64_t size;
} doc_entry_t;

typedef struct
{
    uint32_t total_docs;
    uint32_t pdf_docs;
    uint32_t text_docs;
    uint64_t total_size;
} doc_stats_t;

int doc_init(void);
int doc_list_documents(doc_entry_t *docs, uint32_t *count);
int doc_get_document(const char *doc_name, doc_entry_t *doc);
int doc_search_by_author(const char *author, doc_entry_t *docs, uint32_t *count);
int doc_get_document_content(const char *doc_name, char *content, uint32_t max_size);
int doc_get_stats(doc_stats_t *stats);

#endif
