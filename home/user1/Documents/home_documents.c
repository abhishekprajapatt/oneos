#include "home_documents.h"

int home_documents_init(const char *docs_dir)
{
    if (!docs_dir)
    {
        return -1;
    }
    return 0;
}

int home_documents_create_document(const char *doc_name, uint32_t doc_type)
{
    if (!doc_name)
    {
        return -1;
    }
    return 0;
}

int home_documents_open_document(const char *doc_name)
{
    if (!doc_name)
    {
        return -1;
    }
    return 0;
}

int home_documents_save_document(const char *doc_name, uint8_t *data, uint32_t size)
{
    if (!doc_name || !data)
    {
        return -1;
    }
    return 0;
}

int home_documents_delete_document(const char *doc_name)
{
    if (!doc_name)
    {
        return -1;
    }
    return 0;
}

int home_documents_list_documents(home_documents_file_t *docs, uint32_t max_count)
{
    if (!docs)
    {
        return -1;
    }
    return 0;
}
