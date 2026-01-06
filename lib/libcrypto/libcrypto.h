#ifndef LIBCRYPTO_H
#define LIBCRYPTO_H

#include <stdint.h>

typedef struct
{
    const char *cipher_name;
    uint32_t key_size;
    uint32_t block_size;
} libcrypto_cipher_t;

int libcrypto_init(void);
int libcrypto_encrypt(libcrypto_cipher_t *cipher, const void *plaintext, uint32_t size, void *ciphertext);
int libcrypto_decrypt(libcrypto_cipher_t *cipher, const void *ciphertext, uint32_t size, void *plaintext);
int libcrypto_get_cipher(const char *name, libcrypto_cipher_t *cipher);
int libcrypto_generate_key(uint8_t *key, uint32_t key_size);

#endif
