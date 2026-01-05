#ifndef CRYPTO_QAT_DRIVER_H
#define CRYPTO_QAT_DRIVER_H

#include <stdint.h>

#define QAT_SERVICE_CRYPTO 0
#define QAT_SERVICE_COMPRESS 1

typedef struct
{
    uint32_t device_id;
    uint32_t pci_id;
    uint32_t num_ae;
    uint32_t num_accel;
} crypto_qat_device_t;

typedef struct
{
    uint32_t service;
    uint32_t algorithm;
    uint8_t *key;
    uint32_t key_size;
    uint32_t flags;
} crypto_qat_session_t;

typedef struct
{
    uint8_t *src_buf;
    uint8_t *dst_buf;
    uint32_t src_len;
    uint32_t dst_len;
    uint8_t *iv;
    uint32_t iv_len;
} crypto_qat_request_t;

int crypto_qat_init(void);
int crypto_qat_probe(void);
int crypto_qat_session_create(crypto_qat_device_t *dev, crypto_qat_session_t *session);
int crypto_qat_session_destroy(crypto_qat_device_t *dev, crypto_qat_session_t *session);
int crypto_qat_perform_op(crypto_qat_device_t *dev, crypto_qat_session_t *session, crypto_qat_request_t *req);
int crypto_qat_get_fw_version(crypto_qat_device_t *dev, uint32_t *version);

#endif
