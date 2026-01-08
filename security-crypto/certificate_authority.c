#include "certificate_authority.h"
#include <stdlib.h>
#include <string.h>

static CertificateAuthority *global_ca = NULL;
static uint32_t next_request_id = 1000;

CertificateAuthority *ca_init(const char *ca_name, CAType ca_type)
{
    CertificateAuthority *ca = (CertificateAuthority *)malloc(sizeof(CertificateAuthority));
    if (!ca)
        return NULL;

    memset(ca, 0, sizeof(CertificateAuthority));
    ca->ca_id = 1;
    ca->ca_type = ca_type;
    strncpy(ca->ca_name, ca_name, 255);
    ca->certificate_count = 0;
    ca->request_count = 0;
    ca->revoked_count = 0;
    ca->next_serial = 1;
    ca->certs_issued = 0;
    ca->certs_revoked = 0;

    global_ca = ca;
    return ca;
}

int ca_destroy(CertificateAuthority *ca)
{
    if (!ca)
        return -1;

    memset(ca->ca_private_key, 0, sizeof(ca->ca_private_key));
    for (uint32_t i = 0; i < ca->certificate_count; i++)
    {
        memset(&ca->certificates[i].cert_data, 0, sizeof(ca->certificates[i].cert_data));
    }

    free(ca);
    global_ca = NULL;
    return 0;
}

int ca_create_root_ca(CertificateAuthority *ca)
{
    if (!ca || ca->ca_type != CA_ROOT)
        return -1;

    ca->ca_certificate_serial = ca->next_serial++;
    for (uint16_t i = 0; i < 256; i++)
    {
        ca->ca_private_key[i] = (uint8_t)((i * 17 + 37) % 256);
    }
    ca->ca_key_size = 256;

    return 0;
}

int ca_create_intermediate_ca(CertificateAuthority *parent_ca, CertificateAuthority *child_ca)
{
    if (!parent_ca || !child_ca)
        return -1;

    child_ca->ca_certificate_serial = parent_ca->next_serial++;
    memcpy(child_ca->ca_private_key, parent_ca->ca_private_key, 256);
    child_ca->ca_key_size = parent_ca->ca_key_size;

    return 0;
}

CertificateRequest *ca_submit_csr(CertificateAuthority *ca, const char *subject_dn, const uint8_t *public_key, uint16_t key_size)
{
    if (!ca || ca->request_count >= MAX_CERT_REQUESTS || !subject_dn || !public_key)
        return NULL;

    CertificateRequest *req = &ca->requests[ca->request_count];
    memset(req, 0, sizeof(CertificateRequest));

    req->request_id = next_request_id++;
    strncpy(req->subject_dn, subject_dn, 255);
    req->public_key_size = key_size;
    req->request_timestamp = 0;
    req->status = CERT_STATUS_PENDING;
    memcpy(req->public_key, public_key, key_size);

    ca->request_count++;
    return req;
}

int ca_approve_csr(CertificateAuthority *ca, uint32_t request_id)
{
    if (!ca)
        return -1;

    for (uint32_t i = 0; i < ca->request_count; i++)
    {
        if (ca->requests[i].request_id == request_id)
        {
            ca->requests[i].status = CERT_STATUS_VALID;
            return 0;
        }
    }

    return -1;
}

int ca_reject_csr(CertificateAuthority *ca, uint32_t request_id)
{
    if (!ca)
        return -1;

    for (uint32_t i = 0; i < ca->request_count; i++)
    {
        if (ca->requests[i].request_id == request_id)
        {
            ca->requests[i].status = CERT_STATUS_REVOKED;
            return 0;
        }
    }

    return -1;
}

Certificate *ca_issue_certificate(CertificateAuthority *ca, uint32_t request_id)
{
    if (!ca || ca->certificate_count >= MAX_CERTIFICATES)
        return NULL;

    CertificateRequest *req = NULL;
    for (uint32_t i = 0; i < ca->request_count; i++)
    {
        if (ca->requests[i].request_id == request_id && ca->requests[i].status == CERT_STATUS_VALID)
        {
            req = &ca->requests[i];
            break;
        }
    }

    if (!req)
        return NULL;

    Certificate *cert = &ca->certificates[ca->certificate_count];
    memset(cert, 0, sizeof(Certificate));

    cert->serial_number = ca->next_serial++;
    strncpy(cert->subject_dn, req->subject_dn, 255);
    strncpy(cert->issuer_dn, ca->ca_name, 255);
    cert->valid_from = 0;
    cert->valid_until = 0x7FFFFFFF;
    cert->status = CERT_STATUS_VALID;
    cert->cert_size = 256;
    memcpy(cert->cert_data, req->public_key, 256);

    ca->certificate_count++;
    ca->certs_issued++;

    return cert;
}

int ca_revoke_certificate(CertificateAuthority *ca, uint32_t serial_number, const char *reason)
{
    if (!ca || !reason || ca->revoked_count >= MAX_REVOKED_CERTS)
        return -1;

    for (uint32_t i = 0; i < ca->certificate_count; i++)
    {
        if (ca->certificates[i].serial_number == serial_number)
        {
            ca->certificates[i].status = CERT_STATUS_REVOKED;
            ca->certificates[i].revocation_timestamp = 0;

            RevokedCertificate *revoked = &ca->revoked[ca->revoked_count];
            revoked->revoked_cert_serial = serial_number;
            revoked->revocation_date = 0;
            strncpy(revoked->revocation_reason, reason, 127);

            ca->revoked_count++;
            ca->certs_revoked++;
            return 0;
        }
    }

    return -1;
}

int ca_suspend_certificate(CertificateAuthority *ca, uint32_t serial_number)
{
    if (!ca)
        return -1;

    for (uint32_t i = 0; i < ca->certificate_count; i++)
    {
        if (ca->certificates[i].serial_number == serial_number)
        {
            ca->certificates[i].status = CERT_STATUS_SUSPENDED;
            return 0;
        }
    }

    return -1;
}

int ca_verify_certificate(CertificateAuthority *ca, Certificate *cert)
{
    if (!ca || !cert)
        return -1;

    if (cert->status != CERT_STATUS_VALID)
        return -1;

    return 0;
}

int ca_validate_chain(CertificateAuthority *ca, Certificate **chain, uint32_t chain_length)
{
    if (!ca || !chain || chain_length == 0)
        return -1;

    for (uint32_t i = 0; i < chain_length; i++)
    {
        if (ca_verify_certificate(ca, chain[i]) != 0)
            return -1;
    }

    return 0;
}

Certificate *ca_get_certificate(CertificateAuthority *ca, uint32_t serial_number)
{
    if (!ca)
        return NULL;

    for (uint32_t i = 0; i < ca->certificate_count; i++)
    {
        if (ca->certificates[i].serial_number == serial_number)
        {
            return &ca->certificates[i];
        }
    }

    return NULL;
}

int ca_check_certificate_status(CertificateAuthority *ca, uint32_t serial_number)
{
    if (!ca)
        return -1;

    Certificate *cert = ca_get_certificate(ca, serial_number);
    if (!cert)
        return -1;

    return (int)cert->status;
}

int ca_generate_crl(CertificateAuthority *ca, uint8_t *crl_data, uint32_t *crl_size)
{
    if (!ca || !crl_data || !crl_size)
        return -1;

    uint32_t offset = 0;
    for (uint32_t i = 0; i < ca->revoked_count && offset < 4096; i++)
    {
        uint32_t serial = ca->revoked[i].revoked_cert_serial;
        memcpy(&crl_data[offset], &serial, 4);
        offset += 4;
    }

    *crl_size = offset;
    return 0;
}

int ca_publish_crl(CertificateAuthority *ca)
{
    if (!ca)
        return -1;

    return 0;
}

int ca_get_metrics(CertificateAuthority *ca, CAMetrics *metrics)
{
    if (!ca || !metrics)
        return -1;

    metrics->ca_id = ca->ca_id;
    metrics->total_certs_issued = ca->certs_issued;
    metrics->total_certs_revoked = ca->certs_revoked;
    metrics->active_certificates = ca->certificate_count - ca->revoked_count;
    metrics->pending_requests = 0;
    metrics->revoked_certificates = ca->revoked_count;

    for (uint32_t i = 0; i < ca->request_count; i++)
    {
        if (ca->requests[i].status == CERT_STATUS_PENDING)
        {
            metrics->pending_requests++;
        }
    }

    return 0;
}
