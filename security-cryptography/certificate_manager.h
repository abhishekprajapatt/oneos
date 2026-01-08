#ifndef CERTIFICATE_MANAGER_H
#define CERTIFICATE_MANAGER_H

#include <stdint.h>
#include <stddef.h>
#include <time.h>

#define MAX_CERTIFICATES 512
#define MAX_CERTIFICATE_CHAIN 16
#define MAX_CERTIFICATE_SIZE 4096
#define MAX_KEY_STORES 64
#define MAX_REVOKED_CERTS 256

typedef enum
{
    CERT_TYPE_ROOT_CA,
    CERT_TYPE_INTERMEDIATE_CA,
    CERT_TYPE_END_ENTITY,
    CERT_TYPE_SELF_SIGNED
} CertificateType;

typedef enum
{
    CERT_STATUS_VALID,
    CERT_STATUS_EXPIRED,
    CERT_STATUS_REVOKED,
    CERT_STATUS_SUSPENDED
} CertificateStatus;

typedef struct
{
    uint32_t cert_id;
    char subject[256];
    char issuer[256];
    char thumbprint[64];

    uint8_t *cert_data;
    uint32_t cert_length;

    CertificateType cert_type;
    CertificateStatus status;

    uint64_t issued_time;
    uint64_t expiry_time;
    uint32_t key_bits;

    uint8_t is_trusted;
    uint32_t chain_depth;
} Certificate;

typedef struct
{
    uint32_t key_id;
    char key_alias[64];

    uint8_t *private_key;
    uint32_t private_key_length;

    uint8_t *public_key;
    uint32_t public_key_length;

    uint32_t associated_cert_id;
    uint32_t key_bits;

    uint64_t created_time;
    uint8_t is_protected;
} PrivateKey;

typedef struct
{
    uint32_t store_id;
    char store_name[64];

    Certificate certificates[MAX_CERTIFICATES];
    uint32_t cert_count;

    PrivateKey private_keys[MAX_CERTIFICATES];
    uint32_t key_count;

    uint32_t revoked_cert_ids[MAX_REVOKED_CERTS];
    uint32_t revoked_count;

    uint8_t is_secured;
} KeyStore;

typedef struct
{
    uint32_t manager_id;
    KeyStore key_stores[MAX_KEY_STORES];
    uint32_t store_count;

    uint32_t default_store_id;
    uint64_t certificates_validated;
    uint32_t validation_failures;
} CertificateManager;

CertificateManager *certificate_manager_init(void);
int certificate_manager_destroy(CertificateManager *manager);

uint32_t keystore_create(CertificateManager *manager, const char *store_name);
int keystore_destroy(CertificateManager *manager, uint32_t store_id);

uint32_t certificate_import(CertificateManager *manager, uint32_t store_id,
                            const uint8_t *cert_data, uint32_t cert_length,
                            const char *subject, const char *issuer);

int certificate_export(CertificateManager *manager, uint32_t store_id, uint32_t cert_id,
                       uint8_t *cert_data, uint32_t *cert_length);

int certificate_validate(CertificateManager *manager, uint32_t store_id, uint32_t cert_id);

int certificate_verify_chain(CertificateManager *manager, uint32_t store_id, uint32_t cert_id);

int certificate_revoke(CertificateManager *manager, uint32_t store_id, uint32_t cert_id);

int certificate_check_revocation_status(CertificateManager *manager, uint32_t store_id,
                                        uint32_t cert_id);

int certificate_check_expiry(CertificateManager *manager, uint32_t store_id, uint32_t cert_id);

uint32_t private_key_generate(CertificateManager *manager, uint32_t store_id,
                              uint32_t key_bits, const char *key_alias);

int private_key_import(CertificateManager *manager, uint32_t store_id,
                       const uint8_t *key_data, uint32_t key_length,
                       const char *key_alias);

int private_key_export(CertificateManager *manager, uint32_t store_id, uint32_t key_id,
                       uint8_t *key_data, uint32_t *key_length);

int private_key_protect(CertificateManager *manager, uint32_t store_id, uint32_t key_id);

int private_key_unprotect(CertificateManager *manager, uint32_t store_id, uint32_t key_id);

int certificate_bind_key(CertificateManager *manager, uint32_t store_id,
                         uint32_t cert_id, uint32_t key_id);

int certificate_get_associated_key(CertificateManager *manager, uint32_t store_id,
                                   uint32_t cert_id, uint32_t *key_id);

typedef struct
{
    uint32_t manager_id;
    uint32_t total_stores;
    uint32_t total_certificates;
    uint32_t total_keys;
    uint32_t revoked_certificates;
    uint64_t certificates_validated;
    uint32_t validation_failures;
} CertificateManagerMetrics;

int certificate_manager_get_metrics(CertificateManager *manager, CertificateManagerMetrics *metrics);

#endif
