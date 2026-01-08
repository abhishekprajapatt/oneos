#include "encryption_engine.h"
#include <stdlib.h>
#include <string.h>

static EncryptionEngine *global_encryption_engine = NULL;
static uint32_t next_session_id = 1;
static uint32_t next_buffer_id = 1;

EncryptionEngine *encryption_engine_init(void)
{
    EncryptionEngine *engine = (EncryptionEngine *)malloc(sizeof(EncryptionEngine));
    if (!engine)
        return NULL;

    memset(engine, 0, sizeof(EncryptionEngine));
    engine->engine_id = 1;
    engine->session_count = 0;
    engine->buffer_count = 0;
    engine->total_bytes_encrypted = 0;
    engine->total_bytes_decrypted = 0;
    engine->total_operations = 0;

    global_encryption_engine = engine;
    return engine;
}

int encryption_engine_destroy(EncryptionEngine *engine)
{
    if (!engine)
        return -1;

    for (uint32_t i = 0; i < engine->buffer_count; i++)
    {
        if (engine->buffers[i].encrypted_data)
            free(engine->buffers[i].encrypted_data);
        if (engine->buffers[i].plaintext_data)
            free(engine->buffers[i].plaintext_data);
    }

    free(engine);
    global_encryption_engine = NULL;
    return 0;
}

uint32_t encryption_session_create(EncryptionEngine *engine, CipherType cipher,
                                   EncryptionMode mode)
{
    if (!engine || engine->session_count >= MAX_ENCRYPTION_SESSIONS)
        return 0;

    EncryptionSession *session = &engine->sessions[engine->session_count];
    memset(session, 0, sizeof(EncryptionSession));

    session->session_id = next_session_id++;
    session->cipher_type = cipher;
    session->mode = mode;
    session->bytes_encrypted = 0;
    session->bytes_decrypted = 0;
    session->operation_count = 0;
    session->is_active = 1;

    engine->session_count++;
    return session->session_id;
}

int encryption_session_destroy(EncryptionEngine *engine, uint32_t session_id)
{
    if (!engine || session_id == 0)
        return -1;

    for (uint32_t i = 0; i < engine->session_count; i++)
    {
        if (engine->sessions[i].session_id == session_id)
        {
            memmove(&engine->sessions[i], &engine->sessions[i + 1],
                    sizeof(EncryptionSession) * (engine->session_count - i - 1));
            engine->session_count--;
            return 0;
        }
    }

    return -1;
}

int encryption_key_generate(EncryptionEngine *engine, uint32_t session_id,
                            uint32_t key_length)
{
    if (!engine || session_id == 0 || key_length > MAX_KEY_SIZE)
        return -1;

    for (uint32_t i = 0; i < engine->session_count; i++)
    {
        if (engine->sessions[i].session_id == session_id)
        {
            EncryptionSession *session = &engine->sessions[i];

            session->key.key_length = key_length;
            session->key.created_time = 0;
            session->key.usage_count = 0;
            session->key.is_active = 1;

            for (uint32_t j = 0; j < key_length; j++)
            {
                session->key.key[j] = (uint8_t)((j * 17 + 42) % 256);
            }

            return 0;
        }
    }

    return -1;
}

int encryption_key_import(EncryptionEngine *engine, uint32_t session_id,
                          const uint8_t *key, uint32_t key_length)
{
    if (!engine || session_id == 0 || !key || key_length > MAX_KEY_SIZE)
        return -1;

    for (uint32_t i = 0; i < engine->session_count; i++)
    {
        if (engine->sessions[i].session_id == session_id)
        {
            EncryptionSession *session = &engine->sessions[i];

            memcpy(session->key.key, key, key_length);
            session->key.key_length = key_length;
            session->key.created_time = 0;
            session->key.usage_count = 0;
            session->key.is_active = 1;

            return 0;
        }
    }

    return -1;
}

int encryption_iv_generate(EncryptionEngine *engine, uint32_t session_id,
                           uint32_t iv_length)
{
    if (!engine || session_id == 0 || iv_length > MAX_IV_SIZE)
        return -1;

    for (uint32_t i = 0; i < engine->session_count; i++)
    {
        if (engine->sessions[i].session_id == session_id)
        {
            EncryptionSession *session = &engine->sessions[i];

            session->iv_key.iv_length = iv_length;
            session->iv_key.is_active = 1;

            for (uint32_t j = 0; j < iv_length; j++)
            {
                session->iv_key.iv[j] = (uint8_t)((j * 19 + 73) % 256);
            }

            return 0;
        }
    }

    return -1;
}

int encryption_iv_import(EncryptionEngine *engine, uint32_t session_id,
                         const uint8_t *iv, uint32_t iv_length)
{
    if (!engine || session_id == 0 || !iv || iv_length > MAX_IV_SIZE)
        return -1;

    for (uint32_t i = 0; i < engine->session_count; i++)
    {
        if (engine->sessions[i].session_id == session_id)
        {
            EncryptionSession *session = &engine->sessions[i];

            memcpy(session->iv_key.iv, iv, iv_length);
            session->iv_key.iv_length = iv_length;
            session->iv_key.is_active = 1;

            return 0;
        }
    }

    return -1;
}

int encryption_encrypt_data(EncryptionEngine *engine, uint32_t session_id,
                            const uint8_t *plaintext, uint32_t plaintext_length,
                            uint8_t *ciphertext, uint32_t *ciphertext_length)
{
    if (!engine || session_id == 0 || !plaintext || !ciphertext || !ciphertext_length)
        return -1;

    for (uint32_t i = 0; i < engine->session_count; i++)
    {
        if (engine->sessions[i].session_id == session_id && engine->sessions[i].is_active)
        {
            EncryptionSession *session = &engine->sessions[i];

            *ciphertext_length = plaintext_length + 16;

            for (uint32_t j = 0; j < plaintext_length; j++)
            {
                ciphertext[j] = plaintext[j] ^ session->key.key[j % session->key.key_length];
            }

            session->bytes_encrypted += plaintext_length;
            session->operation_count++;
            engine->total_bytes_encrypted += plaintext_length;
            engine->total_operations++;

            return 0;
        }
    }

    return -1;
}

int encryption_decrypt_data(EncryptionEngine *engine, uint32_t session_id,
                            const uint8_t *ciphertext, uint32_t ciphertext_length,
                            uint8_t *plaintext, uint32_t *plaintext_length)
{
    if (!engine || session_id == 0 || !ciphertext || !plaintext || !plaintext_length)
        return -1;

    for (uint32_t i = 0; i < engine->session_count; i++)
    {
        if (engine->sessions[i].session_id == session_id && engine->sessions[i].is_active)
        {
            EncryptionSession *session = &engine->sessions[i];

            *plaintext_length = ciphertext_length - 16;

            for (uint32_t j = 0; j < *plaintext_length; j++)
            {
                plaintext[j] = ciphertext[j] ^ session->key.key[j % session->key.key_length];
            }

            session->bytes_decrypted += *plaintext_length;
            session->operation_count++;
            engine->total_bytes_decrypted += *plaintext_length;
            engine->total_operations++;

            return 0;
        }
    }

    return -1;
}

int encryption_encrypt_buffer_stream(EncryptionEngine *engine, uint32_t session_id,
                                     const uint8_t *data, uint32_t length)
{
    if (!engine || session_id == 0 || !data || engine->buffer_count >= MAX_ENCRYPTED_BUFFERS)
        return -1;

    EncryptedBuffer *buffer = &engine->buffers[engine->buffer_count];
    memset(buffer, 0, sizeof(EncryptedBuffer));

    buffer->buffer_id = next_buffer_id++;
    buffer->session_id = session_id;
    buffer->plaintext_length = length;

    buffer->plaintext_data = (uint8_t *)malloc(length);
    if (!buffer->plaintext_data)
        return -1;

    memcpy(buffer->plaintext_data, data, length);

    buffer->encrypted_length = length + 16;
    buffer->encrypted_data = (uint8_t *)malloc(buffer->encrypted_length);
    if (!buffer->encrypted_data)
    {
        free(buffer->plaintext_data);
        return -1;
    }

    uint32_t encrypted_len = 0;
    if (encryption_encrypt_data(engine, session_id, data, length,
                                buffer->encrypted_data, &encrypted_len) == 0)
    {
        buffer->encrypted_length = encrypted_len;
        engine->buffer_count++;
        return buffer->buffer_id;
    }

    free(buffer->encrypted_data);
    free(buffer->plaintext_data);
    return -1;
}

int encryption_decrypt_buffer_stream(EncryptionEngine *engine, uint32_t session_id,
                                     uint8_t *data, uint32_t *length)
{
    if (!engine || session_id == 0 || !data || !length)
        return -1;

    for (uint32_t i = 0; i < engine->buffer_count; i++)
    {
        if (engine->buffers[i].session_id == session_id)
        {
            EncryptedBuffer *buffer = &engine->buffers[i];

            if (buffer->plaintext_length <= *length)
            {
                memcpy(data, buffer->plaintext_data, buffer->plaintext_length);
                *length = buffer->plaintext_length;
                return 0;
            }

            return -1;
        }
    }

    return -1;
}

int encryption_rotate_key(EncryptionEngine *engine, uint32_t session_id)
{
    if (!engine || session_id == 0)
        return -1;

    for (uint32_t i = 0; i < engine->session_count; i++)
    {
        if (engine->sessions[i].session_id == session_id)
        {
            EncryptionSession *session = &engine->sessions[i];

            if (session->key.is_active)
            {
                encryption_key_generate(engine, session_id, session->key.key_length);
                return 0;
            }
        }
    }

    return -1;
}

int encryption_destroy_key(EncryptionEngine *engine, uint32_t session_id)
{
    if (!engine || session_id == 0)
        return -1;

    for (uint32_t i = 0; i < engine->session_count; i++)
    {
        if (engine->sessions[i].session_id == session_id)
        {
            EncryptionSession *session = &engine->sessions[i];

            memset(session->key.key, 0, sizeof(session->key.key));
            session->key.is_active = 0;
            return 0;
        }
    }

    return -1;
}

int encryption_compute_hash(const uint8_t *data, uint32_t length,
                            uint8_t *hash, uint32_t hash_length)
{
    if (!data || !hash || length == 0 || hash_length == 0)
        return -1;

    uint64_t hash_value = 5381;

    for (uint32_t i = 0; i < length; i++)
    {
        hash_value = ((hash_value << 5) + hash_value) + data[i];
    }

    for (uint32_t i = 0; i < hash_length; i++)
    {
        hash[i] = (uint8_t)((hash_value >> (i * 8)) % 256);
    }

    return 0;
}

int encryption_engine_get_metrics(EncryptionEngine *engine, EncryptionEngineMetrics *metrics)
{
    if (!engine || !metrics)
        return -1;

    uint32_t active_sessions = 0;

    for (uint32_t i = 0; i < engine->session_count; i++)
    {
        if (engine->sessions[i].is_active)
        {
            active_sessions++;
        }
    }

    metrics->engine_id = engine->engine_id;
    metrics->active_sessions = active_sessions;
    metrics->total_operations = engine->total_operations;
    metrics->total_bytes_encrypted = engine->total_bytes_encrypted;
    metrics->total_bytes_decrypted = engine->total_bytes_decrypted;

    if (engine->total_operations > 0)
    {
        metrics->encryption_throughput_mbps = (float)(engine->total_bytes_encrypted + engine->total_bytes_decrypted) / (engine->total_operations * 1024.0f);
    }
    else
    {
        metrics->encryption_throughput_mbps = 0.0f;
    }

    return 0;
}
