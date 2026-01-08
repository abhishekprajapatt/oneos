#ifndef TLS_MANAGER_H
#define TLS_MANAGER_H

#include <stdint.h>
#include <stddef.h>

#define MAX_TLS_SESSIONS 512
#define MAX_TLS_CERTIFICATES 256
#define MAX_CIPHERS_PER_SESSION 8
#define MAX_HANDSHAKE_BUFFER 8192

typedef enum
{
    TLS_VERSION_1_0,
    TLS_VERSION_1_1,
    TLS_VERSION_1_2,
    TLS_VERSION_1_3
} TLSVersion;

typedef enum
{
    TLS_STATE_IDLE,
    TLS_STATE_HANDSHAKE,
    TLS_STATE_ESTABLISHED,
    TLS_STATE_CLOSING,
    TLS_STATE_CLOSED,
    TLS_STATE_ERROR
} TLSState;

typedef enum
{
    CIPHER_SUITE_AES_128_GCM_SHA256,
    CIPHER_SUITE_AES_256_GCM_SHA384,
    CIPHER_SUITE_CHACHA20_POLY1305,
    CIPHER_SUITE_RSA_AES_128_CBC_SHA,
    CIPHER_SUITE_ECDHE_AES_256_GCM_SHA384
} CipherSuite;

typedef struct
{
    uint32_t session_id;
    uint32_t connection_id;

    TLSVersion tls_version;
    TLSState state;
    CipherSuite negotiated_cipher;

    uint8_t client_random[32];
    uint8_t server_random[32];
    uint8_t session_ticket[32];

    uint64_t bytes_sent;
    uint64_t bytes_received;
    uint64_t session_start_time;
    uint64_t last_activity_time;

    uint32_t handshake_count;
    uint32_t renegotiation_count;
} TLSSession;

typedef struct
{
    uint32_t alert_id;
    uint32_t session_id;
    uint8_t alert_level;
    uint8_t alert_description;
    uint64_t alert_timestamp;
} TLSAlert;

typedef struct
{
    uint32_t manager_id;
    TLSSession sessions[MAX_TLS_SESSIONS];
    uint32_t session_count;

    TLSAlert alerts[256];
    uint32_t alert_count;

    CipherSuite supported_ciphers[MAX_CIPHERS_PER_SESSION];
    uint32_t cipher_count;

    TLSVersion min_version;
    TLSVersion max_version;

    uint64_t total_bytes_encrypted;
    uint64_t total_bytes_decrypted;
    uint32_t total_handshakes;
    uint32_t failed_handshakes;
} TLSManager;

TLSManager *tls_manager_init(void);
int tls_manager_destroy(TLSManager *manager);

uint32_t tls_session_create(TLSManager *manager, uint32_t connection_id, TLSVersion version);
int tls_session_destroy(TLSManager *manager, uint32_t session_id);

int tls_session_start_handshake(TLSManager *manager, uint32_t session_id);
int tls_session_handle_client_hello(TLSManager *manager, uint32_t session_id,
                                    const uint8_t *hello_data, uint32_t hello_length);
int tls_session_handle_server_hello(TLSManager *manager, uint32_t session_id,
                                    const uint8_t *hello_data, uint32_t hello_length);

int tls_cipher_suite_add(TLSManager *manager, CipherSuite cipher);
int tls_cipher_suite_remove(TLSManager *manager, CipherSuite cipher);
int tls_cipher_suite_negotiate(TLSManager *manager, uint32_t session_id,
                               const CipherSuite *client_ciphers, uint32_t cipher_count);

int tls_session_set_state(TLSManager *manager, uint32_t session_id, TLSState state);
int tls_session_get_state(TLSManager *manager, uint32_t session_id, TLSState *state);

int tls_record_encrypt(TLSManager *manager, uint32_t session_id,
                       const uint8_t *plaintext, uint32_t plaintext_length,
                       uint8_t *ciphertext, uint32_t *ciphertext_length);

int tls_record_decrypt(TLSManager *manager, uint32_t session_id,
                       const uint8_t *ciphertext, uint32_t ciphertext_length,
                       uint8_t *plaintext, uint32_t *plaintext_length);

int tls_send_alert(TLSManager *manager, uint32_t session_id,
                   uint8_t alert_level, uint8_t alert_description);

int tls_verify_certificate_chain(TLSManager *manager, uint32_t session_id);

int tls_session_resume(TLSManager *manager, uint32_t session_id);
int tls_session_renegotiate(TLSManager *manager, uint32_t session_id);

int tls_set_min_version(TLSManager *manager, TLSVersion version);
int tls_set_max_version(TLSManager *manager, TLSVersion version);

typedef struct
{
    uint32_t manager_id;
    uint32_t active_sessions;
    uint32_t total_sessions;
    uint32_t supported_ciphers;
    uint64_t total_bytes_encrypted;
    uint64_t total_bytes_decrypted;
    uint32_t total_handshakes;
    uint32_t failed_handshakes;
} TLSManagerMetrics;

int tls_manager_get_metrics(TLSManager *manager, TLSManagerMetrics *metrics);

#endif
