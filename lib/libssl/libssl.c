#include "libssl.h"

int libssl_init(void)
{
    return 0;
}

int libssl_CTX_new(SSL_CTX **ctx)
{
    if (!ctx)
    {
        return -1;
    }
    return 0;
}

int libssl_CTX_free(SSL_CTX *ctx)
{
    if (!ctx)
    {
        return -1;
    }
    return 0;
}

int libssl_new(SSL_CTX *ctx, SSL **ssl)
{
    if (!ctx || !ssl)
    {
        return -1;
    }
    return 0;
}

int libssl_free(SSL *ssl)
{
    if (!ssl)
    {
        return -1;
    }
    return 0;
}

int libssl_connect(SSL *ssl)
{
    if (!ssl)
    {
        return -1;
    }
    return 0;
}

int libssl_read(SSL *ssl, void *buf, uint32_t len, int *read_len)
{
    if (!ssl || !buf || len == 0 || !read_len)
    {
        return -1;
    }
    return 0;
}

int libssl_write(SSL *ssl, const void *buf, uint32_t len, int *written_len)
{
    if (!ssl || !buf || len == 0 || !written_len)
    {
        return -1;
    }
    return 0;
}
