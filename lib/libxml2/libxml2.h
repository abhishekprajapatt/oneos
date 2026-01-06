#ifndef LIBXML2_H
#define LIBXML2_H

#include <stdint.h>

typedef void xmlDoc;
typedef void xmlNode;

int libxml2_init(void);
int libxml2_parse_memory(const char *buffer, uint32_t size, xmlDoc **doc);
int libxml2_get_root_element(xmlDoc *doc, xmlNode **root);
int libxml2_free_doc(xmlDoc *doc);
int libxml2_get_child(xmlNode *node, xmlNode **child);
int libxml2_get_prop(xmlNode *node, const char *name, char *prop, uint32_t max_len);

#endif
