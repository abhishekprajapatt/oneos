#ifndef LIBPAC_H
#define LIBPAC_H

#include <stdint.h>

typedef void pac_engine_t;

int libpac_init(void);
int libpac_engine_new(pac_engine_t **engine);
int libpac_engine_free(pac_engine_t *engine);
int libpac_engine_load(pac_engine_t *engine, const char *pac_url);
int libpac_find_proxy(pac_engine_t *engine, const char *url, char *proxy, uint32_t max_len);

#endif
