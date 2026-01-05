#include "libreoffice.h"

int libreoffice_init(void)
{
    return 0;
}

int libreoffice_open_document(const char *document_path)
{
    if (!document_path)
    {
        return -1;
    }
    return 0;
}

int libreoffice_convert_document(const char *input, const char *output, const char *format)
{
    if (!input || !output || !format)
    {
        return -1;
    }
    return 0;
}

int libreoffice_convert_with_options(libreoffice_convert_options_t *options)
{
    if (!options || !options->document_path || !options->output_format)
    {
        return -1;
    }
    return 0;
}

int libreoffice_get_document_info(const char *document, char *info, uint32_t info_size)
{
    if (!document || !info || info_size == 0)
    {
        return -1;
    }
    return 0;
}

int libreoffice_get_stats(libreoffice_stats_t *stats)
{
    if (!stats)
    {
        return -1;
    }
    return 0;
}
