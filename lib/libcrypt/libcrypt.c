#include "libcrypt.h"

int libcrypt_init(void)
{
    return 0;
}

int libcrypt_crypt(const char *key, const char *salt, char *result, uint32_t max_len)
{
    if (!key || !salt || !result || max_len == 0)
    {
        return -1;
    }
    return 0;
}

int libcrypt_crypt_r(const char *key, const char *salt, char *result, uint32_t max_len)
{
    if (!key || !salt || !result || max_len == 0)
    {
        return -1;
    }
    return 0;
}

int libcrypt_md5_hash(const void *data, uint32_t size, char *hash, uint32_t hash_len)
{
    if (!data || size == 0 || !hash || hash_len == 0)
    {
        return -1;
    }
    return 0;
}

int libcrypt_sha256_hash(const void *data, uint32_t size, char *hash, uint32_t hash_len)
{
    if (!data || size == 0 || !hash || hash_len == 0)
    {
        return -1;
    }
    return 0;
}
