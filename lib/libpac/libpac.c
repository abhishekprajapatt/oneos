#include "libpac.h"

int libpac_init(void)
{
    return 0;
}

int libpac_engine_new(pac_engine_t **engine)
{
    if (!engine)
    {
        return -1;
    }
    return 0;
}

int libpac_engine_free(pac_engine_t *engine)
{
    if (!engine)
    {
        return -1;
    }
    return 0;
}

int libpac_engine_load(pac_engine_t *engine, const char *pac_url)
{
    if (!engine || !pac_url)
    {
        return -1;
    }
    return 0;
}

int libpac_find_proxy(pac_engine_t *engine, const char *url, char *proxy, uint32_t max_len)
{
    if (!engine || !url || !proxy || max_len == 0)
    {
        return -1;
    }
    return 0;
}
