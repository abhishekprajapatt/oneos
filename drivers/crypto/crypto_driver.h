#ifndef CRYPTO_DRIVER_H
#define CRYPTO_DRIVER_H

#include <stdint.h>

#define CRYPTO_ALG_AES 1
#define CRYPTO_ALG_SHA256 2
#define CRYPTO_MODE_ECB 0
#define CRYPTO_MODE_CBC 1

typedef struct
{
    uint32_t algorithm;
    uint32_t mode;
    uint8_t key[32];
    uint32_t key_size;
} crypto_config_t;

typedef struct
{
    uint8_t *input;
    uint8_t *output;
    uint32_t length;
} crypto_request_t;

int crypto_init(void);
int crypto_encrypt(crypto_config_t *cfg, crypto_request_t *req);
int crypto_decrypt(crypto_config_t *cfg, crypto_request_t *req);
int crypto_hash(uint8_t *input, uint32_t size, uint8_t *output);

#endif
