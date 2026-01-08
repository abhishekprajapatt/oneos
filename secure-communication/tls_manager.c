#include "tls_manager.h"
#include <stdlib.h>
#include <string.h>

static TLSManager *global_tls_manager = NULL;
static uint32_t next_session_id = 1;
static uint32_t next_alert_id = 1;

TLSManager *tls_manager_init(void)
{
    TLSManager *manager = (TLSManager *)malloc(sizeof(TLSManager));
    if (!manager)
        return NULL;

    memset(manager, 0, sizeof(TLSManager));
    manager->manager_id = 1;
    manager->session_count = 0;
    manager->alert_count = 0;
    manager->cipher_count = 0;

    manager->min_version = TLS_VERSION_1_2;
    manager->max_version = TLS_VERSION_1_3;

    manager->total_bytes_encrypted = 0;
    manager->total_bytes_decrypted = 0;
    manager->total_handshakes = 0;
    manager->failed_handshakes = 0;

    global_tls_manager = manager;
    return manager;
}

int tls_manager_destroy(TLSManager *manager)
{
    if (!manager)
        return -1;

    free(manager);
    global_tls_manager = NULL;
    return 0;
}

uint32_t tls_session_create(TLSManager *manager, uint32_t connection_id, TLSVersion version)
{
    if (!manager || manager->session_count >= MAX_TLS_SESSIONS)
        return 0;

    TLSSession *session = &manager->sessions[manager->session_count];
    memset(session, 0, sizeof(TLSSession));

    session->session_id = next_session_id++;
    session->connection_id = connection_id;
    session->tls_version = version;
    session->state = TLS_STATE_IDLE;
    session->negotiated_cipher = CIPHER_SUITE_AES_128_GCM_SHA256;

    session->bytes_sent = 0;
    session->bytes_received = 0;
    session->session_start_time = 0;
    session->last_activity_time = 0;
    session->handshake_count = 0;
    session->renegotiation_count = 0;

    for (uint32_t i = 0; i < 32; i++)
    {
        session->client_random[i] = (uint8_t)((i * 11 + 73) % 256);
        session->server_random[i] = (uint8_t)((i * 13 + 97) % 256);
    }

    manager->session_count++;
    return session->session_id;
}

int tls_session_destroy(TLSManager *manager, uint32_t session_id)
{
    if (!manager || session_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->session_count; i++)
    {
        if (manager->sessions[i].session_id == session_id)
        {
            memmove(&manager->sessions[i], &manager->sessions[i + 1],
                    sizeof(TLSSession) * (manager->session_count - i - 1));
            manager->session_count--;
            return 0;
        }
    }

    return -1;
}

int tls_session_start_handshake(TLSManager *manager, uint32_t session_id)
{
    if (!manager || session_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->session_count; i++)
    {
        if (manager->sessions[i].session_id == session_id)
        {
            TLSSession *session = &manager->sessions[i];

            if (session->state == TLS_STATE_IDLE || session->state == TLS_STATE_ESTABLISHED)
            {
                session->state = TLS_STATE_HANDSHAKE;
                session->handshake_count++;
                manager->total_handshakes++;
                return 0;
            }

            return -1;
        }
    }

    return -1;
}

int tls_session_handle_client_hello(TLSManager *manager, uint32_t session_id,
                                    const uint8_t *hello_data, uint32_t hello_length)
{
    if (!manager || session_id == 0 || !hello_data)
        return -1;

    for (uint32_t i = 0; i < manager->session_count; i++)
    {
        if (manager->sessions[i].session_id == session_id)
        {
            TLSSession *session = &manager->sessions[i];

            if (session->state == TLS_STATE_HANDSHAKE)
            {
                session->bytes_received += hello_length;
                return 0;
            }

            return -1;
        }
    }

    return -1;
}

int tls_session_handle_server_hello(TLSManager *manager, uint32_t session_id,
                                    const uint8_t *hello_data, uint32_t hello_length)
{
    if (!manager || session_id == 0 || !hello_data)
        return -1;

    for (uint32_t i = 0; i < manager->session_count; i++)
    {
        if (manager->sessions[i].session_id == session_id)
        {
            TLSSession *session = &manager->sessions[i];

            if (session->state == TLS_STATE_HANDSHAKE)
            {
                session->bytes_sent += hello_length;
                return 0;
            }

            return -1;
        }
    }

    return -1;
}

int tls_cipher_suite_add(TLSManager *manager, CipherSuite cipher)
{
    if (!manager || manager->cipher_count >= MAX_CIPHERS_PER_SESSION)
        return -1;

    manager->supported_ciphers[manager->cipher_count++] = cipher;
    return 0;
}

int tls_cipher_suite_remove(TLSManager *manager, CipherSuite cipher)
{
    if (!manager)
        return -1;

    for (uint32_t i = 0; i < manager->cipher_count; i++)
    {
        if (manager->supported_ciphers[i] == cipher)
        {
            memmove(&manager->supported_ciphers[i], &manager->supported_ciphers[i + 1],
                    sizeof(CipherSuite) * (manager->cipher_count - i - 1));
            manager->cipher_count--;
            return 0;
        }
    }

    return -1;
}

int tls_cipher_suite_negotiate(TLSManager *manager, uint32_t session_id,
                               const CipherSuite *client_ciphers, uint32_t cipher_count)
{
    if (!manager || session_id == 0 || !client_ciphers || cipher_count == 0)
        return -1;

    for (uint32_t i = 0; i < manager->session_count; i++)
    {
        if (manager->sessions[i].session_id == session_id)
        {
            TLSSession *session = &manager->sessions[i];

            for (uint32_t j = 0; j < cipher_count && j < manager->cipher_count; j++)
            {
                for (uint32_t k = 0; k < manager->cipher_count; k++)
                {
                    if (client_ciphers[j] == manager->supported_ciphers[k])
                    {
                        session->negotiated_cipher = client_ciphers[j];
                        return 0;
                    }
                }
            }

            return -1;
        }
    }

    return -1;
}

int tls_session_set_state(TLSManager *manager, uint32_t session_id, TLSState state)
{
    if (!manager || session_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->session_count; i++)
    {
        if (manager->sessions[i].session_id == session_id)
        {
            manager->sessions[i].state = state;
            return 0;
        }
    }

    return -1;
}

int tls_session_get_state(TLSManager *manager, uint32_t session_id, TLSState *state)
{
    if (!manager || session_id == 0 || !state)
        return -1;

    for (uint32_t i = 0; i < manager->session_count; i++)
    {
        if (manager->sessions[i].session_id == session_id)
        {
            *state = manager->sessions[i].state;
            return 0;
        }
    }

    return -1;
}

int tls_record_encrypt(TLSManager *manager, uint32_t session_id,
                       const uint8_t *plaintext, uint32_t plaintext_length,
                       uint8_t *ciphertext, uint32_t *ciphertext_length)
{
    if (!manager || session_id == 0 || !plaintext || !ciphertext || !ciphertext_length)
        return -1;

    for (uint32_t i = 0; i < manager->session_count; i++)
    {
        if (manager->sessions[i].session_id == session_id &&
            manager->sessions[i].state == TLS_STATE_ESTABLISHED)
        {

            TLSSession *session = &manager->sessions[i];

            *ciphertext_length = plaintext_length + 16;

            for (uint32_t j = 0; j < plaintext_length; j++)
            {
                ciphertext[j] = plaintext[j] ^ (j % 256);
            }

            session->bytes_sent += plaintext_length;
            manager->total_bytes_encrypted += plaintext_length;
            return 0;
        }
    }

    return -1;
}

int tls_record_decrypt(TLSManager *manager, uint32_t session_id,
                       const uint8_t *ciphertext, uint32_t ciphertext_length,
                       uint8_t *plaintext, uint32_t *plaintext_length)
{
    if (!manager || session_id == 0 || !ciphertext || !plaintext || !plaintext_length)
        return -1;

    for (uint32_t i = 0; i < manager->session_count; i++)
    {
        if (manager->sessions[i].session_id == session_id &&
            manager->sessions[i].state == TLS_STATE_ESTABLISHED)
        {

            TLSSession *session = &manager->sessions[i];

            *plaintext_length = ciphertext_length - 16;

            for (uint32_t j = 0; j < *plaintext_length; j++)
            {
                plaintext[j] = ciphertext[j] ^ (j % 256);
            }

            session->bytes_received += *plaintext_length;
            manager->total_bytes_decrypted += *plaintext_length;
            return 0;
        }
    }

    return -1;
}

int tls_send_alert(TLSManager *manager, uint32_t session_id,
                   uint8_t alert_level, uint8_t alert_description)
{
    if (!manager || session_id == 0 || manager->alert_count >= 256)
        return -1;

    TLSAlert *alert = &manager->alerts[manager->alert_count];
    memset(alert, 0, sizeof(TLSAlert));

    alert->alert_id = next_alert_id++;
    alert->session_id = session_id;
    alert->alert_level = alert_level;
    alert->alert_description = alert_description;
    alert->alert_timestamp = 0;

    manager->alert_count++;
    return alert->alert_id;
}

int tls_verify_certificate_chain(TLSManager *manager, uint32_t session_id)
{
    if (!manager || session_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->session_count; i++)
    {
        if (manager->sessions[i].session_id == session_id)
        {
            return 0;
        }
    }

    return -1;
}

int tls_session_resume(TLSManager *manager, uint32_t session_id)
{
    if (!manager || session_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->session_count; i++)
    {
        if (manager->sessions[i].session_id == session_id)
        {
            TLSSession *session = &manager->sessions[i];

            if (session->state == TLS_STATE_IDLE || session->state == TLS_STATE_CLOSED)
            {
                session->state = TLS_STATE_ESTABLISHED;
                return 0;
            }

            return -1;
        }
    }

    return -1;
}

int tls_session_renegotiate(TLSManager *manager, uint32_t session_id)
{
    if (!manager || session_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->session_count; i++)
    {
        if (manager->sessions[i].session_id == session_id)
        {
            TLSSession *session = &manager->sessions[i];

            if (session->state == TLS_STATE_ESTABLISHED)
            {
                session->state = TLS_STATE_HANDSHAKE;
                session->renegotiation_count++;
                manager->total_handshakes++;
                return 0;
            }

            return -1;
        }
    }

    return -1;
}

int tls_set_min_version(TLSManager *manager, TLSVersion version)
{
    if (!manager)
        return -1;

    manager->min_version = version;
    return 0;
}

int tls_set_max_version(TLSManager *manager, TLSVersion version)
{
    if (!manager)
        return -1;

    manager->max_version = version;
    return 0;
}

int tls_manager_get_metrics(TLSManager *manager, TLSManagerMetrics *metrics)
{
    if (!manager || !metrics)
        return -1;

    uint32_t active_sessions = 0;

    for (uint32_t i = 0; i < manager->session_count; i++)
    {
        if (manager->sessions[i].state == TLS_STATE_ESTABLISHED ||
            manager->sessions[i].state == TLS_STATE_HANDSHAKE)
        {
            active_sessions++;
        }
    }

    metrics->manager_id = manager->manager_id;
    metrics->active_sessions = active_sessions;
    metrics->total_sessions = manager->session_count;
    metrics->supported_ciphers = manager->cipher_count;
    metrics->total_bytes_encrypted = manager->total_bytes_encrypted;
    metrics->total_bytes_decrypted = manager->total_bytes_decrypted;
    metrics->total_handshakes = manager->total_handshakes;
    metrics->failed_handshakes = manager->failed_handshakes;

    return 0;
}
