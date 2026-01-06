#ifndef LIBCRYPT_H
#define LIBCRYPT_H

#include <stdint.h>

int libcrypt_init(void);
int libcrypt_crypt(const char *key, const char *salt, char *result, uint32_t max_len);
int libcrypt_crypt_r(const char *key, const char *salt, char *result, uint32_t max_len);
int libcrypt_md5_hash(const void *data, uint32_t size, char *hash, uint32_t hash_len);
int libcrypt_sha256_hash(const void *data, uint32_t size, char *hash, uint32_t hash_len);

#endif
