#ifndef LOCAL_HEADERS_H
#define LOCAL_HEADERS_H

#include <stdint.h>

int local_headers_init(void);
int local_headers_register_header(const char *header_name, const char *header_path);
int local_headers_unregister_header(const char *header_name);
int local_headers_list_headers(char **headers, uint32_t max_headers);
int local_headers_find_header(const char *header_name, char *path, uint32_t max_len);
int local_headers_install_custom_header(const char *header_file);

#endif
