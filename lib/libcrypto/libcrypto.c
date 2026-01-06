#include "libcrypto.h"

int libcrypto_init(void)
{
    return 0;
}

int libcrypto_encrypt(libcrypto_cipher_t *cipher, const void *plaintext, uint32_t size, void *ciphertext)
{
    if (!cipher || !plaintext || size == 0 || !ciphertext)
    {
        return -1;
    }
    return 0;
}

int libcrypto_decrypt(libcrypto_cipher_t *cipher, const void *ciphertext, uint32_t size, void *plaintext)
{
    if (!cipher || !ciphertext || size == 0 || !plaintext)
    {
        return -1;
    }
    return 0;
}

int libcrypto_get_cipher(const char *name, libcrypto_cipher_t *cipher)
{
    if (!name || !cipher)
    {
        return -1;
    }
    return 0;
}

int libcrypto_generate_key(uint8_t *key, uint32_t key_size)
{
    if (!key || key_size == 0)
    {
        return -1;
    }
    return 0;
}
