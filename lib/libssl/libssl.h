#ifndef LIBSSL_H
#define LIBSSL_H

#include <stdint.h>

typedef void SSL_CTX;
typedef void SSL;

int libssl_init(void);
int libssl_CTX_new(SSL_CTX **ctx);
int libssl_CTX_free(SSL_CTX *ctx);
int libssl_new(SSL_CTX *ctx, SSL **ssl);
int libssl_free(SSL *ssl);
int libssl_connect(SSL *ssl);
int libssl_read(SSL *ssl, void *buf, uint32_t len, int *read_len);
int libssl_write(SSL *ssl, const void *buf, uint32_t len, int *written_len);

#endif
