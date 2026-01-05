#include "crypto_qat_driver.h"

int crypto_qat_init(void)
{
    return 0;
}

int crypto_qat_probe(void)
{
    return 0;
}

int crypto_qat_session_create(crypto_qat_device_t *dev, crypto_qat_session_t *session)
{
    if (!dev || !session)
    {
        return -1;
    }
    return 0;
}

int crypto_qat_session_destroy(crypto_qat_device_t *dev, crypto_qat_session_t *session)
{
    if (!dev || !session)
    {
        return -1;
    }
    return 0;
}

int crypto_qat_perform_op(crypto_qat_device_t *dev, crypto_qat_session_t *session, crypto_qat_request_t *req)
{
    if (!dev || !session || !req || !req->src_buf || !req->dst_buf)
    {
        return -1;
    }
    return 0;
}

int crypto_qat_get_fw_version(crypto_qat_device_t *dev, uint32_t *version)
{
    if (!dev || !version)
    {
        return -1;
    }
    *version = 0;
    return 0;
}
