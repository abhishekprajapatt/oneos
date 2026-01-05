#include "texmaker.h"

int texmaker_init(void)
{
    return 0;
}

int texmaker_open_document(const char *document_path)
{
    if (!document_path)
    {
        return -1;
    }
    return 0;
}

int texmaker_create_new_document(void)
{
    return 0;
}

int texmaker_compile_document(const char *document_path)
{
    if (!document_path)
    {
        return -1;
    }
    return 0;
}

int texmaker_get_document_info(texmaker_document_info_t *info)
{
    if (!info)
    {
        return -1;
    }
    return 0;
}

int texmaker_view_pdf(const char *pdf_path)
{
    if (!pdf_path)
    {
        return -1;
    }
    return 0;
}
