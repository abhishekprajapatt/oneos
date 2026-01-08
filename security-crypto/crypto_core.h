#ifndef CRYPTO_CORE_H
#define CRYPTO_CORE_H

#include <stdint.h>
#include <stddef.h>

#define MAX_CRYPTO_KEYS 512
#define MAX_CIPHER_SUITES 32
#define MAX_HASH_ALGORITHMS 16
#define MAX_ENCRYPTION_JOBS 256

typedef enum
{
    CRYPTO_AES_128,
    CRYPTO_AES_192,
    CRYPTO_AES_256,
    CRYPTO_CHACHA20,
    CRYPTO_3DES,
    CRYPTO_RSA_2048,
    CRYPTO_RSA_4096,
    CRYPTO_ECC_P256,
    CRYPTO_ECC_P384
} CryptoAlgorithm;

typedef enum
{
    MODE_ECB,
    MODE_CBC,
    MODE_CTR,
    MODE_GCM,
    MODE_XTS
} CipherMode;

typedef enum
{
    HASH_SHA256,
    HASH_SHA384,
    HASH_SHA512,
    HASH_SHA3_256,
    HASH_SHA3_512,
    HASH_BLAKE2B,
    HASH_BLAKE3
} HashAlgorithm;

typedef struct
{
    uint32_t key_id;
    CryptoAlgorithm algorithm;
    uint8_t key_material[256];
    uint16_t key_size;
    uint32_t created_timestamp;
    uint32_t expires_timestamp;
    uint8_t is_active;
} CryptoKey;

typedef struct
{
    uint32_t cipher_suite_id;
    CryptoAlgorithm encryption_algo;
    CipherMode mode;
    HashAlgorithm hash_algo;
    uint16_t key_size;
    uint16_t iv_size;
} CipherSuite;

typedef struct
{
    uint32_t job_id;
    uint32_t key_id;
    CipherSuite *cipher_suite;
    uint8_t *plaintext;
    uint64_t plaintext_len;
    uint8_t *ciphertext;
    uint64_t ciphertext_len;
    uint8_t iv[16];
    uint8_t aad[256];
    uint64_t aad_len;
    uint8_t tag[16];
    uint32_t status;
} EncryptionJob;

typedef struct
{
    uint32_t engine_id;
    uint32_t key_count;
    CryptoKey keys[MAX_CRYPTO_KEYS];
    uint32_t cipher_suite_count;
    CipherSuite cipher_suites[MAX_CIPHER_SUITES];
    uint32_t job_count;
    EncryptionJob jobs[MAX_ENCRYPTION_JOBS];
    uint64_t total_bytes_encrypted;
    uint64_t total_bytes_decrypted;
} CryptoEngine;

CryptoEngine *crypto_engine_init(void);
int crypto_engine_destroy(CryptoEngine *engine);

CryptoKey *crypto_create_key(CryptoEngine *engine, CryptoAlgorithm algo, uint16_t key_size);
int crypto_destroy_key(CryptoEngine *engine, uint32_t key_id);
int crypto_import_key(CryptoEngine *engine, const uint8_t *key_material, uint16_t key_size);
int crypto_export_key(CryptoEngine *engine, uint32_t key_id, uint8_t *key_material);

int crypto_encrypt(CryptoEngine *engine, uint32_t key_id, uint8_t *plaintext, uint64_t plen, uint8_t *ciphertext);
int crypto_decrypt(CryptoEngine *engine, uint32_t key_id, uint8_t *ciphertext, uint64_t clen, uint8_t *plaintext);

int crypto_hash(HashAlgorithm algo, const uint8_t *data, uint64_t data_len, uint8_t *digest);
int crypto_hmac(HashAlgorithm algo, const uint8_t *key, uint16_t key_len, const uint8_t *data, uint64_t data_len, uint8_t *mac);

CipherSuite *crypto_register_cipher_suite(CryptoEngine *engine, CryptoAlgorithm enc, CipherMode mode, HashAlgorithm hash);
int crypto_select_cipher_suite(CryptoEngine *engine, uint32_t suite_id);

#endif
