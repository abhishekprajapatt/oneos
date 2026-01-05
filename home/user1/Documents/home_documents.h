#ifndef HOME_DOCUMENTS_H
#define HOME_DOCUMENTS_H

#include <stdint.h>

typedef struct
{
    uint32_t doc_id;
    const char *document_name;
    uint32_t doc_type;
    uint64_t doc_size;
    uint64_t created_time;
} home_documents_file_t;

typedef struct
{
    uint32_t document_count;
    uint64_t total_size;
    uint32_t version;
} home_documents_state_t;

int home_documents_init(const char *docs_dir);
int home_documents_create_document(const char *doc_name, uint32_t doc_type);
int home_documents_open_document(const char *doc_name);
int home_documents_save_document(const char *doc_name, uint8_t *data, uint32_t size);
int home_documents_delete_document(const char *doc_name);
int home_documents_list_documents(home_documents_file_t *docs, uint32_t max_count);

#endif
