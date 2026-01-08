#ifndef PKI_MANAGEMENT_H
#define PKI_MANAGEMENT_H

#include <stdint.h>
#include <stddef.h>

#define MAX_CERTIFICATE_CHAINS 256
#define MAX_PUBLIC_KEYS 512
#define MAX_PRIVATE_KEYS 256
#define MAX_KEY_PAIRS 256

typedef enum
{
    KEY_TYPE_RSA,
    KEY_TYPE_ECC,
    KEY_TYPE_DSA,
    KEY_TYPE_ED25519
} KeyType;

typedef enum
{
    SIGNATURE_RSA_SHA256,
    SIGNATURE_RSA_SHA512,
    SIGNATURE_ECDSA_SHA256,
    SIGNATURE_ED25519
} SignatureAlgorithm;

typedef struct
{
    uint32_t public_key_id;
    KeyType key_type;
    uint8_t key_data[512];
    uint16_t key_size;
    uint32_t created_timestamp;
} PublicKey;

typedef struct
{
    uint32_t private_key_id;
    KeyType key_type;
    uint8_t key_data[1024];
    uint16_t key_size;
    uint32_t created_timestamp;
    uint8_t is_encrypted;
} PrivateKey;

typedef struct
{
    uint32_t key_pair_id;
    PublicKey *public_key;
    PrivateKey *private_key;
    uint32_t created_timestamp;
    uint32_t expires_timestamp;
} KeyPair;

typedef struct
{
    uint32_t chain_id;
    KeyPair *key_pair;
    uint8_t *certificate_chain[16];
    uint32_t chain_length;
    uint8_t is_trusted;
} CertificateChain;

typedef struct
{
    uint32_t pki_manager_id;
    uint32_t public_key_count;
    PublicKey public_keys[MAX_PUBLIC_KEYS];
    uint32_t private_key_count;
    PrivateKey private_keys[MAX_PRIVATE_KEYS];
    uint32_t key_pair_count;
    KeyPair key_pairs[MAX_KEY_PAIRS];
    uint32_t chain_count;
    CertificateChain chains[MAX_CERTIFICATE_CHAINS];
} PKIManager;

PKIManager *pki_manager_init(void);
int pki_manager_destroy(PKIManager *manager);

KeyPair *pki_generate_key_pair(PKIManager *manager, KeyType type, uint16_t key_size);
int pki_import_public_key(PKIManager *manager, const uint8_t *key_data, uint16_t key_size);
int pki_import_private_key(PKIManager *manager, const uint8_t *key_data, uint16_t key_size);

int pki_sign_data(PKIManager *manager, uint32_t private_key_id, const uint8_t *data, uint64_t data_len, uint8_t *signature);
int pki_verify_signature(PKIManager *manager, uint32_t public_key_id, const uint8_t *data, uint64_t data_len, const uint8_t *signature);

CertificateChain *pki_create_certificate_chain(PKIManager *manager, uint32_t key_pair_id);
int pki_add_certificate_to_chain(PKIManager *manager, uint32_t chain_id, const uint8_t *cert_data);
int pki_validate_certificate_chain(PKIManager *manager, uint32_t chain_id);

int pki_export_public_key(PKIManager *manager, uint32_t public_key_id, uint8_t *key_data);
int pki_export_private_key(PKIManager *manager, uint32_t private_key_id, uint8_t *key_data);

#endif
