#ifndef LIBXSLT_H
#define LIBXSLT_H

#include <stdint.h>

typedef void xsltStylesheet;
typedef void xmlDoc;

int libxslt_init(void);
int libxslt_parse_stylesheet(xmlDoc *doc, xsltStylesheet **stylesheet);
int libxslt_free_stylesheet(xsltStylesheet *stylesheet);
int libxslt_apply_stylesheet(xsltStylesheet *stylesheet, xmlDoc *doc, xmlDoc **result);
int libxslt_process(const char *xml_file, const char *xsl_file, char *output, uint32_t output_size);

#endif
