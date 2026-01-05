#ifndef CRYPTO_CAAM_DRIVER_H
#define CRYPTO_CAAM_DRIVER_H

#include <stdint.h>

#define CAAM_ALG_AES 1
#define CAAM_ALG_DES 2
#define CAAM_ALG_SHA256 3
#define CAAM_ALG_SHA512 4

typedef struct
{
    uint32_t device_id;
    uint32_t version;
    uint32_t deco_count;
    uint32_t qi_present;
} crypto_caam_device_t;

typedef struct
{
    uint32_t algorithm;
    uint32_t mode;
    uint8_t *key;
    uint32_t key_size;
} crypto_caam_config_t;

typedef struct
{
    uint8_t *input;
    uint8_t *output;
    uint32_t length;
    uint8_t *iv;
    uint32_t iv_size;
} crypto_caam_request_t;

int crypto_caam_init(void);
int crypto_caam_probe(void);
int crypto_caam_encrypt(crypto_caam_device_t *dev, crypto_caam_config_t *cfg, crypto_caam_request_t *req);
int crypto_caam_decrypt(crypto_caam_device_t *dev, crypto_caam_config_t *cfg, crypto_caam_request_t *req);
int crypto_caam_hash(crypto_caam_device_t *dev, uint32_t alg, uint8_t *input, uint32_t len, uint8_t *output);
int crypto_caam_rng_generate(crypto_caam_device_t *dev, uint8_t *buffer, uint32_t size);

#endif
