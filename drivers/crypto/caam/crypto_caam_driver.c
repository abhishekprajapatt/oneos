#include "crypto_caam_driver.h"

int crypto_caam_init(void)
{
    return 0;
}

int crypto_caam_probe(void)
{
    return 0;
}

int crypto_caam_encrypt(crypto_caam_device_t *dev, crypto_caam_config_t *cfg, crypto_caam_request_t *req)
{
    if (!dev || !cfg || !req || !req->input || !req->output)
    {
        return -1;
    }
    return 0;
}

int crypto_caam_decrypt(crypto_caam_device_t *dev, crypto_caam_config_t *cfg, crypto_caam_request_t *req)
{
    if (!dev || !cfg || !req || !req->input || !req->output)
    {
        return -1;
    }
    return 0;
}

int crypto_caam_hash(crypto_caam_device_t *dev, uint32_t alg, uint8_t *input, uint32_t len, uint8_t *output)
{
    if (!dev || !input || !output || len == 0)
    {
        return -1;
    }
    return 0;
}

int crypto_caam_rng_generate(crypto_caam_device_t *dev, uint8_t *buffer, uint32_t size)
{
    if (!dev || !buffer || size == 0)
    {
        return -1;
    }
    return 0;
}
