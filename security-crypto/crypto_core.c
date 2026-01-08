#include "crypto_core.h"
#include <stdlib.h>
#include <string.h>

static CryptoEngine *global_crypto = NULL;
static uint32_t next_key_id = 1;
static uint32_t next_job_id = 1;

CryptoEngine *crypto_engine_init(void)
{
    CryptoEngine *engine = (CryptoEngine *)malloc(sizeof(CryptoEngine));
    if (!engine)
        return NULL;

    memset(engine, 0, sizeof(CryptoEngine));
    engine->engine_id = 1;
    engine->key_count = 0;
    engine->cipher_suite_count = 0;
    engine->job_count = 0;
    engine->total_bytes_encrypted = 0;
    engine->total_bytes_decrypted = 0;

    global_crypto = engine;
    return engine;
}

int crypto_engine_destroy(CryptoEngine *engine)
{
    if (!engine)
        return -1;

    for (uint32_t i = 0; i < engine->key_count; i++)
    {
        memset(&engine->keys[i].key_material, 0, sizeof(engine->keys[i].key_material));
    }

    free(engine);
    global_crypto = NULL;
    return 0;
}

CryptoKey *crypto_create_key(CryptoEngine *engine, CryptoAlgorithm algo, uint16_t key_size)
{
    if (!engine || engine->key_count >= MAX_CRYPTO_KEYS)
        return NULL;

    CryptoKey *key = &engine->keys[engine->key_count];
    memset(key, 0, sizeof(CryptoKey));

    key->key_id = next_key_id++;
    key->algorithm = algo;
    key->key_size = key_size;
    key->created_timestamp = 0;
    key->expires_timestamp = 0;
    key->is_active = 1;

    // Generate random key material (simulated)
    for (uint16_t i = 0; i < key_size && i < 256; i++)
    {
        key->key_material[i] = (uint8_t)((i * 7 + 13) % 256);
    }

    engine->key_count++;
    return key;
}

int crypto_destroy_key(CryptoEngine *engine, uint32_t key_id)
{
    if (!engine)
        return -1;

    for (uint32_t i = 0; i < engine->key_count; i++)
    {
        if (engine->keys[i].key_id == key_id)
        {
            memset(&engine->keys[i].key_material, 0, sizeof(engine->keys[i].key_material));
            engine->keys[i].is_active = 0;
            return 0;
        }
    }

    return -1;
}

int crypto_import_key(CryptoEngine *engine, const uint8_t *key_material, uint16_t key_size)
{
    if (!engine || !key_material || key_size == 0 || key_size > 256)
        return -1;

    if (engine->key_count >= MAX_CRYPTO_KEYS)
        return -1;

    CryptoKey *key = &engine->keys[engine->key_count];
    memset(key, 0, sizeof(CryptoKey));

    key->key_id = next_key_id++;
    key->key_size = key_size;
    key->is_active = 1;
    memcpy(key->key_material, key_material, key_size);

    engine->key_count++;
    return 0;
}

int crypto_export_key(CryptoEngine *engine, uint32_t key_id, uint8_t *key_material)
{
    if (!engine || !key_material)
        return -1;

    for (uint32_t i = 0; i < engine->key_count; i++)
    {
        if (engine->keys[i].key_id == key_id && engine->keys[i].is_active)
        {
            memcpy(key_material, engine->keys[i].key_material, engine->keys[i].key_size);
            return 0;
        }
    }

    return -1;
}

int crypto_encrypt(CryptoEngine *engine, uint32_t key_id, uint8_t *plaintext, uint64_t plen, uint8_t *ciphertext)
{
    if (!engine || !plaintext || !ciphertext || plen == 0)
        return -1;

    for (uint32_t i = 0; i < engine->key_count; i++)
    {
        if (engine->keys[i].key_id == key_id && engine->keys[i].is_active)
        {
            // Simulated XOR encryption
            for (uint64_t j = 0; j < plen; j++)
            {
                ciphertext[j] = plaintext[j] ^ engine->keys[i].key_material[j % engine->keys[i].key_size];
            }
            engine->total_bytes_encrypted += plen;
            return 0;
        }
    }

    return -1;
}

int crypto_decrypt(CryptoEngine *engine, uint32_t key_id, uint8_t *ciphertext, uint64_t clen, uint8_t *plaintext)
{
    if (!engine || !ciphertext || !plaintext || clen == 0)
        return -1;

    for (uint32_t i = 0; i < engine->key_count; i++)
    {
        if (engine->keys[i].key_id == key_id && engine->keys[i].is_active)
        {
            // Simulated XOR decryption
            for (uint64_t j = 0; j < clen; j++)
            {
                plaintext[j] = ciphertext[j] ^ engine->keys[i].key_material[j % engine->keys[i].key_size];
            }
            engine->total_bytes_decrypted += clen;
            return 0;
        }
    }

    return -1;
}

int crypto_hash(HashAlgorithm algo, const uint8_t *data, uint64_t data_len, uint8_t *digest)
{
    if (!data || !digest || data_len == 0)
        return -1;

    uint32_t hash_val = 0;
    for (uint64_t i = 0; i < data_len; i++)
    {
        hash_val = ((hash_val << 5) + hash_val) + data[i];
    }

    memcpy(digest, &hash_val, 4);
    return 0;
}

int crypto_hmac(HashAlgorithm algo, const uint8_t *key, uint16_t key_len, const uint8_t *data, uint64_t data_len, uint8_t *mac)
{
    if (!key || !data || !mac || key_len == 0 || data_len == 0)
        return -1;

    uint32_t hmac_val = 0;
    for (uint16_t i = 0; i < key_len; i++)
    {
        hmac_val ^= key[i];
    }
    for (uint64_t i = 0; i < data_len; i++)
    {
        hmac_val = ((hmac_val << 5) + hmac_val) + data[i];
    }

    memcpy(mac, &hmac_val, 4);
    return 0;
}

CipherSuite *crypto_register_cipher_suite(CryptoEngine *engine, CryptoAlgorithm enc, CipherMode mode, HashAlgorithm hash)
{
    if (!engine || engine->cipher_suite_count >= MAX_CIPHER_SUITES)
        return NULL;

    CipherSuite *suite = &engine->cipher_suites[engine->cipher_suite_count];
    memset(suite, 0, sizeof(CipherSuite));

    suite->cipher_suite_id = engine->cipher_suite_count;
    suite->encryption_algo = enc;
    suite->mode = mode;
    suite->hash_algo = hash;
    suite->key_size = 256;
    suite->iv_size = 16;

    engine->cipher_suite_count++;
    return suite;
}

int crypto_select_cipher_suite(CryptoEngine *engine, uint32_t suite_id)
{
    if (!engine || suite_id >= engine->cipher_suite_count)
        return -1;

    return 0;
}
