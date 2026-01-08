#ifndef CERTIFICATE_AUTHORITY_H
#define CERTIFICATE_AUTHORITY_H

#include <stdint.h>
#include <stddef.h>

#define MAX_CERTIFICATES 1024
#define MAX_CA_HIERARCHY 8
#define MAX_CERT_REQUESTS 512
#define MAX_REVOKED_CERTS 2048

typedef enum
{
    CERT_STATUS_VALID,
    CERT_STATUS_EXPIRED,
    CERT_STATUS_REVOKED,
    CERT_STATUS_SUSPENDED,
    CERT_STATUS_PENDING
} CertificateStatus;

typedef enum
{
    CA_ROOT,
    CA_INTERMEDIATE,
    CA_SUBORDINATE
} CAType;

typedef struct
{
    uint32_t serial_number;
    char subject_dn[256];
    char issuer_dn[256];
    uint32_t valid_from;
    uint32_t valid_until;
    CertificateStatus status;
    uint8_t cert_data[2048];
    uint32_t cert_size;
    uint32_t revocation_timestamp;
} Certificate;

typedef struct
{
    uint32_t request_id;
    char subject_dn[256];
    uint8_t public_key[512];
    uint16_t public_key_size;
    uint32_t request_timestamp;
    CertificateStatus status;
} CertificateRequest;

typedef struct
{
    uint32_t revoked_cert_serial;
    uint32_t revocation_date;
    char revocation_reason[128];
} RevokedCertificate;

typedef struct
{
    uint32_t ca_id;
    CAType ca_type;
    char ca_name[256];
    uint32_t ca_certificate_serial;
    uint8_t ca_private_key[1024];
    uint16_t ca_key_size;

    uint32_t certificate_count;
    Certificate certificates[MAX_CERTIFICATES];

    uint32_t request_count;
    CertificateRequest requests[MAX_CERT_REQUESTS];

    uint32_t revoked_count;
    RevokedCertificate revoked[MAX_REVOKED_CERTS];

    uint32_t next_serial;
    uint64_t certs_issued;
    uint64_t certs_revoked;
} CertificateAuthority;

CertificateAuthority *ca_init(const char *ca_name, CAType ca_type);
int ca_destroy(CertificateAuthority *ca);

int ca_create_root_ca(CertificateAuthority *ca);
int ca_create_intermediate_ca(CertificateAuthority *parent_ca, CertificateAuthority *child_ca);

CertificateRequest *ca_submit_csr(CertificateAuthority *ca, const char *subject_dn, const uint8_t *public_key, uint16_t key_size);
int ca_approve_csr(CertificateAuthority *ca, uint32_t request_id);
int ca_reject_csr(CertificateAuthority *ca, uint32_t request_id);

Certificate *ca_issue_certificate(CertificateAuthority *ca, uint32_t request_id);
int ca_revoke_certificate(CertificateAuthority *ca, uint32_t serial_number, const char *reason);
int ca_suspend_certificate(CertificateAuthority *ca, uint32_t serial_number);

int ca_verify_certificate(CertificateAuthority *ca, Certificate *cert);
int ca_validate_chain(CertificateAuthority *ca, Certificate **chain, uint32_t chain_length);

Certificate *ca_get_certificate(CertificateAuthority *ca, uint32_t serial_number);
int ca_check_certificate_status(CertificateAuthority *ca, uint32_t serial_number);

int ca_generate_crl(CertificateAuthority *ca, uint8_t *crl_data, uint32_t *crl_size);
int ca_publish_crl(CertificateAuthority *ca);

typedef struct
{
    uint32_t ca_id;
    uint64_t total_certs_issued;
    uint64_t total_certs_revoked;
    uint32_t active_certificates;
    uint32_t pending_requests;
    uint32_t revoked_certificates;
} CAMetrics;

int ca_get_metrics(CertificateAuthority *ca, CAMetrics *metrics);

#endif
