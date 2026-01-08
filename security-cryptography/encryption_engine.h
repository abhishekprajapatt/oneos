#ifndef ENCRYPTION_ENGINE_H
#define ENCRYPTION_ENGINE_H

#include <stdint.h>
#include <stddef.h>

#define MAX_ENCRYPTION_SESSIONS 256
#define MAX_KEY_SIZE 256
#define MAX_IV_SIZE 16
#define MAX_CIPHER_BLOCK_SIZE 256
#define MAX_ENCRYPTED_BUFFERS 1024

typedef enum
{
    CIPHER_AES_128,
    CIPHER_AES_256,
    CIPHER_RSA_2048,
    CIPHER_RSA_4096,
    CIPHER_CHACHA20,
    CIPHER_3DES
} CipherType;

typedef enum
{
    MODE_ECB,
    MODE_CBC,
    MODE_CTR,
    MODE_GCM,
    MODE_XTS
} EncryptionMode;

typedef struct
{
    uint8_t key[MAX_KEY_SIZE];
    uint32_t key_length;
    uint8_t iv[MAX_IV_SIZE];
    uint32_t iv_length;

    uint64_t created_time;
    uint32_t usage_count;
    uint8_t is_active;
} EncryptionKey;

typedef struct
{
    uint32_t session_id;
    CipherType cipher_type;
    EncryptionMode mode;

    EncryptionKey key;
    EncryptionKey iv_key;

    uint64_t bytes_encrypted;
    uint64_t bytes_decrypted;
    uint32_t operation_count;

    uint8_t is_active;
} EncryptionSession;

typedef struct
{
    uint32_t buffer_id;
    uint8_t *encrypted_data;
    uint32_t encrypted_length;

    uint8_t *plaintext_data;
    uint32_t plaintext_length;

    uint32_t session_id;
    CipherType cipher_type;
} EncryptedBuffer;

typedef struct
{
    uint32_t engine_id;
    EncryptionSession sessions[MAX_ENCRYPTION_SESSIONS];
    uint32_t session_count;

    EncryptedBuffer buffers[MAX_ENCRYPTED_BUFFERS];
    uint32_t buffer_count;

    uint64_t total_bytes_encrypted;
    uint64_t total_bytes_decrypted;
    uint32_t total_operations;
} EncryptionEngine;

EncryptionEngine *encryption_engine_init(void);
int encryption_engine_destroy(EncryptionEngine *engine);

uint32_t encryption_session_create(EncryptionEngine *engine, CipherType cipher,
                                   EncryptionMode mode);
int encryption_session_destroy(EncryptionEngine *engine, uint32_t session_id);

int encryption_key_generate(EncryptionEngine *engine, uint32_t session_id,
                            uint32_t key_length);
int encryption_key_import(EncryptionEngine *engine, uint32_t session_id,
                          const uint8_t *key, uint32_t key_length);

int encryption_iv_generate(EncryptionEngine *engine, uint32_t session_id,
                           uint32_t iv_length);
int encryption_iv_import(EncryptionEngine *engine, uint32_t session_id,
                         const uint8_t *iv, uint32_t iv_length);

int encryption_encrypt_data(EncryptionEngine *engine, uint32_t session_id,
                            const uint8_t *plaintext, uint32_t plaintext_length,
                            uint8_t *ciphertext, uint32_t *ciphertext_length);

int encryption_decrypt_data(EncryptionEngine *engine, uint32_t session_id,
                            const uint8_t *ciphertext, uint32_t ciphertext_length,
                            uint8_t *plaintext, uint32_t *plaintext_length);

int encryption_encrypt_buffer_stream(EncryptionEngine *engine, uint32_t session_id,
                                     const uint8_t *data, uint32_t length);

int encryption_decrypt_buffer_stream(EncryptionEngine *engine, uint32_t session_id,
                                     uint8_t *data, uint32_t *length);

int encryption_rotate_key(EncryptionEngine *engine, uint32_t session_id);
int encryption_destroy_key(EncryptionEngine *engine, uint32_t session_id);

int encryption_compute_hash(const uint8_t *data, uint32_t length,
                            uint8_t *hash, uint32_t hash_length);

typedef struct
{
    uint32_t engine_id;
    uint32_t active_sessions;
    uint32_t total_operations;
    uint64_t total_bytes_encrypted;
    uint64_t total_bytes_decrypted;
    float encryption_throughput_mbps;
} EncryptionEngineMetrics;

int encryption_engine_get_metrics(EncryptionEngine *engine, EncryptionEngineMetrics *metrics);

#endif
