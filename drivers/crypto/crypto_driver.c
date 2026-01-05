#include "crypto_driver.h"

int crypto_init(void)
{
    return 0;
}

int crypto_encrypt(crypto_config_t *cfg, crypto_request_t *req)
{
    if (!cfg || !req || !req->input || !req->output)
    {
        return -1;
    }
    return 0;
}

int crypto_decrypt(crypto_config_t *cfg, crypto_request_t *req)
{
    if (!cfg || !req || !req->input || !req->output)
    {
        return -1;
    }
    return 0;
}

int crypto_hash(uint8_t *input, uint32_t size, uint8_t *output)
{
    if (!input || !output || size == 0)
    {
        return -1;
    }
    return 0;
}
