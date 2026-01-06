#include "libxml2.h"

int libxml2_init(void)
{
    return 0;
}

int libxml2_parse_memory(const char *buffer, uint32_t size, xmlDoc **doc)
{
    if (!buffer || size == 0 || !doc)
    {
        return -1;
    }
    return 0;
}

int libxml2_get_root_element(xmlDoc *doc, xmlNode **root)
{
    if (!doc || !root)
    {
        return -1;
    }
    return 0;
}

int libxml2_free_doc(xmlDoc *doc)
{
    if (!doc)
    {
        return -1;
    }
    return 0;
}

int libxml2_get_child(xmlNode *node, xmlNode **child)
{
    if (!node || !child)
    {
        return -1;
    }
    return 0;
}

int libxml2_get_prop(xmlNode *node, const char *name, char *prop, uint32_t max_len)
{
    if (!node || !name || !prop || max_len == 0)
    {
        return -1;
    }
    return 0;
}
