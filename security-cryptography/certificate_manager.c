#include "certificate_manager.h"
#include <stdlib.h>
#include <string.h>

static CertificateManager *global_certificate_manager = NULL;
static uint32_t next_cert_id = 1;
static uint32_t next_key_id = 1;
static uint32_t next_store_id = 1;

CertificateManager *certificate_manager_init(void)
{
    CertificateManager *manager = (CertificateManager *)malloc(sizeof(CertificateManager));
    if (!manager)
        return NULL;

    memset(manager, 0, sizeof(CertificateManager));
    manager->manager_id = 1;
    manager->store_count = 0;
    manager->default_store_id = 0;
    manager->certificates_validated = 0;
    manager->validation_failures = 0;

    global_certificate_manager = manager;
    return manager;
}

int certificate_manager_destroy(CertificateManager *manager)
{
    if (!manager)
        return -1;

    for (uint32_t i = 0; i < manager->store_count; i++)
    {
        for (uint32_t j = 0; j < manager->key_stores[i].cert_count; j++)
        {
            if (manager->key_stores[i].certificates[j].cert_data)
                free(manager->key_stores[i].certificates[j].cert_data);
        }

        for (uint32_t j = 0; j < manager->key_stores[i].key_count; j++)
        {
            if (manager->key_stores[i].private_keys[j].private_key)
                free(manager->key_stores[i].private_keys[j].private_key);
            if (manager->key_stores[i].private_keys[j].public_key)
                free(manager->key_stores[i].private_keys[j].public_key);
        }
    }

    free(manager);
    global_certificate_manager = NULL;
    return 0;
}

uint32_t keystore_create(CertificateManager *manager, const char *store_name)
{
    if (!manager || manager->store_count >= MAX_KEY_STORES)
        return 0;

    KeyStore *store = &manager->key_stores[manager->store_count];
    memset(store, 0, sizeof(KeyStore));

    store->store_id = next_store_id++;

    if (store_name)
    {
        strncpy(store->store_name, store_name, sizeof(store->store_name) - 1);
    }

    store->cert_count = 0;
    store->key_count = 0;
    store->revoked_count = 0;
    store->is_secured = 1;

    if (manager->default_store_id == 0)
    {
        manager->default_store_id = store->store_id;
    }

    manager->store_count++;
    return store->store_id;
}

int keystore_destroy(CertificateManager *manager, uint32_t store_id)
{
    if (!manager || store_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->store_count; i++)
    {
        if (manager->key_stores[i].store_id == store_id)
        {
            memmove(&manager->key_stores[i], &manager->key_stores[i + 1],
                    sizeof(KeyStore) * (manager->store_count - i - 1));
            manager->store_count--;

            if (manager->default_store_id == store_id)
            {
                manager->default_store_id = 0;
            }

            return 0;
        }
    }

    return -1;
}

uint32_t certificate_import(CertificateManager *manager, uint32_t store_id,
                            const uint8_t *cert_data, uint32_t cert_length,
                            const char *subject, const char *issuer)
{
    if (!manager || store_id == 0 || !cert_data)
        return 0;

    for (uint32_t i = 0; i < manager->store_count; i++)
    {
        if (manager->key_stores[i].store_id == store_id)
        {
            KeyStore *store = &manager->key_stores[i];

            if (store->cert_count >= MAX_CERTIFICATES)
                return 0;

            Certificate *cert = &store->certificates[store->cert_count];
            memset(cert, 0, sizeof(Certificate));

            cert->cert_id = next_cert_id++;

            if (subject)
                strncpy(cert->subject, subject, sizeof(cert->subject) - 1);
            if (issuer)
                strncpy(cert->issuer, issuer, sizeof(cert->issuer) - 1);

            cert->cert_data = (uint8_t *)malloc(cert_length);
            if (!cert->cert_data)
                return 0;

            memcpy(cert->cert_data, cert_data, cert_length);
            cert->cert_length = cert_length;

            cert->status = CERT_STATUS_VALID;
            cert->cert_type = CERT_TYPE_END_ENTITY;
            cert->key_bits = 2048;
            cert->is_trusted = 0;
            cert->chain_depth = 0;
            cert->issued_time = 0;
            cert->expiry_time = 0;

            snprintf(cert->thumbprint, sizeof(cert->thumbprint), "THUMB_%u", cert->cert_id);

            store->cert_count++;
            manager->certificates_validated++;

            return cert->cert_id;
        }
    }

    return 0;
}

int certificate_export(CertificateManager *manager, uint32_t store_id, uint32_t cert_id,
                       uint8_t *cert_data, uint32_t *cert_length)
{
    if (!manager || store_id == 0 || cert_id == 0 || !cert_data || !cert_length)
        return -1;

    for (uint32_t i = 0; i < manager->store_count; i++)
    {
        if (manager->key_stores[i].store_id == store_id)
        {
            KeyStore *store = &manager->key_stores[i];

            for (uint32_t j = 0; j < store->cert_count; j++)
            {
                if (store->certificates[j].cert_id == cert_id)
                {
                    Certificate *cert = &store->certificates[j];

                    if (cert->cert_length <= *cert_length)
                    {
                        memcpy(cert_data, cert->cert_data, cert->cert_length);
                        *cert_length = cert->cert_length;
                        return 0;
                    }

                    return -1;
                }
            }
        }
    }

    return -1;
}

int certificate_validate(CertificateManager *manager, uint32_t store_id, uint32_t cert_id)
{
    if (!manager || store_id == 0 || cert_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->store_count; i++)
    {
        if (manager->key_stores[i].store_id == store_id)
        {
            KeyStore *store = &manager->key_stores[i];

            for (uint32_t j = 0; j < store->cert_count; j++)
            {
                if (store->certificates[j].cert_id == cert_id)
                {
                    Certificate *cert = &store->certificates[j];

                    if (cert->status == CERT_STATUS_VALID)
                    {
                        manager->certificates_validated++;
                        return 0;
                    }
                    else
                    {
                        manager->validation_failures++;
                        return -1;
                    }
                }
            }
        }
    }

    return -1;
}

int certificate_verify_chain(CertificateManager *manager, uint32_t store_id, uint32_t cert_id)
{
    if (!manager || store_id == 0 || cert_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->store_count; i++)
    {
        if (manager->key_stores[i].store_id == store_id)
        {
            KeyStore *store = &manager->key_stores[i];

            for (uint32_t j = 0; j < store->cert_count; j++)
            {
                if (store->certificates[j].cert_id == cert_id)
                {
                    Certificate *cert = &store->certificates[j];

                    if (cert->chain_depth <= MAX_CERTIFICATE_CHAIN)
                    {
                        cert->chain_depth++;
                        return 0;
                    }

                    return -1;
                }
            }
        }
    }

    return -1;
}

int certificate_revoke(CertificateManager *manager, uint32_t store_id, uint32_t cert_id)
{
    if (!manager || store_id == 0 || cert_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->store_count; i++)
    {
        if (manager->key_stores[i].store_id == store_id)
        {
            KeyStore *store = &manager->key_stores[i];

            for (uint32_t j = 0; j < store->cert_count; j++)
            {
                if (store->certificates[j].cert_id == cert_id)
                {
                    Certificate *cert = &store->certificates[j];
                    cert->status = CERT_STATUS_REVOKED;

                    if (store->revoked_count < MAX_REVOKED_CERTS)
                    {
                        store->revoked_cert_ids[store->revoked_count++] = cert_id;
                    }

                    return 0;
                }
            }
        }
    }

    return -1;
}

int certificate_check_revocation_status(CertificateManager *manager, uint32_t store_id,
                                        uint32_t cert_id)
{
    if (!manager || store_id == 0 || cert_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->store_count; i++)
    {
        if (manager->key_stores[i].store_id == store_id)
        {
            KeyStore *store = &manager->key_stores[i];

            for (uint32_t j = 0; j < store->revoked_count; j++)
            {
                if (store->revoked_cert_ids[j] == cert_id)
                {
                    return 1;
                }
            }

            return 0;
        }
    }

    return -1;
}

int certificate_check_expiry(CertificateManager *manager, uint32_t store_id, uint32_t cert_id)
{
    if (!manager || store_id == 0 || cert_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->store_count; i++)
    {
        if (manager->key_stores[i].store_id == store_id)
        {
            KeyStore *store = &manager->key_stores[i];

            for (uint32_t j = 0; j < store->cert_count; j++)
            {
                if (store->certificates[j].cert_id == cert_id)
                {
                    Certificate *cert = &store->certificates[j];

                    if (cert->status == CERT_STATUS_EXPIRED)
                    {
                        return 1;
                    }

                    return 0;
                }
            }
        }
    }

    return -1;
}

uint32_t private_key_generate(CertificateManager *manager, uint32_t store_id,
                              uint32_t key_bits, const char *key_alias)
{
    if (!manager || store_id == 0 || key_bits == 0)
        return 0;

    for (uint32_t i = 0; i < manager->store_count; i++)
    {
        if (manager->key_stores[i].store_id == store_id)
        {
            KeyStore *store = &manager->key_stores[i];

            if (store->key_count >= MAX_CERTIFICATES)
                return 0;

            PrivateKey *key = &store->private_keys[store->key_count];
            memset(key, 0, sizeof(PrivateKey));

            key->key_id = next_key_id++;
            key->key_bits = key_bits;
            key->created_time = 0;
            key->is_protected = 1;

            if (key_alias)
            {
                strncpy(key->key_alias, key_alias, sizeof(key->key_alias) - 1);
            }

            key->private_key_length = key_bits / 8;
            key->private_key = (uint8_t *)malloc(key->private_key_length);
            if (!key->private_key)
                return 0;

            for (uint32_t j = 0; j < key->private_key_length; j++)
            {
                key->private_key[j] = (uint8_t)((j * 23 + 89) % 256);
            }

            key->public_key_length = key_bits / 8;
            key->public_key = (uint8_t *)malloc(key->public_key_length);
            if (!key->public_key)
            {
                free(key->private_key);
                return 0;
            }

            for (uint32_t j = 0; j < key->public_key_length; j++)
            {
                key->public_key[j] = (uint8_t)((j * 31 + 127) % 256);
            }

            store->key_count++;
            return key->key_id;
        }
    }

    return 0;
}

int private_key_import(CertificateManager *manager, uint32_t store_id,
                       const uint8_t *key_data, uint32_t key_length,
                       const char *key_alias)
{
    if (!manager || store_id == 0 || !key_data)
        return -1;

    for (uint32_t i = 0; i < manager->store_count; i++)
    {
        if (manager->key_stores[i].store_id == store_id)
        {
            KeyStore *store = &manager->key_stores[i];

            if (store->key_count >= MAX_CERTIFICATES)
                return -1;

            PrivateKey *key = &store->private_keys[store->key_count];
            memset(key, 0, sizeof(PrivateKey));

            key->key_id = next_key_id++;

            if (key_alias)
            {
                strncpy(key->key_alias, key_alias, sizeof(key->key_alias) - 1);
            }

            key->private_key_length = key_length;
            key->private_key = (uint8_t *)malloc(key_length);
            if (!key->private_key)
                return -1;

            memcpy(key->private_key, key_data, key_length);
            key->key_bits = key_length * 8;
            key->is_protected = 1;

            store->key_count++;
            return key->key_id;
        }
    }

    return -1;
}

int private_key_export(CertificateManager *manager, uint32_t store_id, uint32_t key_id,
                       uint8_t *key_data, uint32_t *key_length)
{
    if (!manager || store_id == 0 || key_id == 0 || !key_data || !key_length)
        return -1;

    for (uint32_t i = 0; i < manager->store_count; i++)
    {
        if (manager->key_stores[i].store_id == store_id)
        {
            KeyStore *store = &manager->key_stores[i];

            for (uint32_t j = 0; j < store->key_count; j++)
            {
                if (store->private_keys[j].key_id == key_id)
                {
                    PrivateKey *key = &store->private_keys[j];

                    if (key->private_key_length <= *key_length)
                    {
                        memcpy(key_data, key->private_key, key->private_key_length);
                        *key_length = key->private_key_length;
                        return 0;
                    }

                    return -1;
                }
            }
        }
    }

    return -1;
}

int private_key_protect(CertificateManager *manager, uint32_t store_id, uint32_t key_id)
{
    if (!manager || store_id == 0 || key_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->store_count; i++)
    {
        if (manager->key_stores[i].store_id == store_id)
        {
            KeyStore *store = &manager->key_stores[i];

            for (uint32_t j = 0; j < store->key_count; j++)
            {
                if (store->private_keys[j].key_id == key_id)
                {
                    store->private_keys[j].is_protected = 1;
                    return 0;
                }
            }
        }
    }

    return -1;
}

int private_key_unprotect(CertificateManager *manager, uint32_t store_id, uint32_t key_id)
{
    if (!manager || store_id == 0 || key_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->store_count; i++)
    {
        if (manager->key_stores[i].store_id == store_id)
        {
            KeyStore *store = &manager->key_stores[i];

            for (uint32_t j = 0; j < store->key_count; j++)
            {
                if (store->private_keys[j].key_id == key_id)
                {
                    store->private_keys[j].is_protected = 0;
                    return 0;
                }
            }
        }
    }

    return -1;
}

int certificate_bind_key(CertificateManager *manager, uint32_t store_id,
                         uint32_t cert_id, uint32_t key_id)
{
    if (!manager || store_id == 0 || cert_id == 0 || key_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->store_count; i++)
    {
        if (manager->key_stores[i].store_id == store_id)
        {
            KeyStore *store = &manager->key_stores[i];

            for (uint32_t j = 0; j < store->cert_count; j++)
            {
                if (store->certificates[j].cert_id == cert_id)
                {
                    store->certificates[j].chain_depth = key_id;
                    return 0;
                }
            }
        }
    }

    return -1;
}

int certificate_get_associated_key(CertificateManager *manager, uint32_t store_id,
                                   uint32_t cert_id, uint32_t *key_id)
{
    if (!manager || store_id == 0 || cert_id == 0 || !key_id)
        return -1;

    for (uint32_t i = 0; i < manager->store_count; i++)
    {
        if (manager->key_stores[i].store_id == store_id)
        {
            KeyStore *store = &manager->key_stores[i];

            for (uint32_t j = 0; j < store->cert_count; j++)
            {
                if (store->certificates[j].cert_id == cert_id)
                {
                    *key_id = store->certificates[j].chain_depth;
                    return 0;
                }
            }
        }
    }

    return -1;
}

int certificate_manager_get_metrics(CertificateManager *manager, CertificateManagerMetrics *metrics)
{
    if (!manager || !metrics)
        return -1;

    uint32_t total_certs = 0;
    uint32_t total_keys = 0;
    uint32_t revoked = 0;

    for (uint32_t i = 0; i < manager->store_count; i++)
    {
        total_certs += manager->key_stores[i].cert_count;
        total_keys += manager->key_stores[i].key_count;
        revoked += manager->key_stores[i].revoked_count;
    }

    metrics->manager_id = manager->manager_id;
    metrics->total_stores = manager->store_count;
    metrics->total_certificates = total_certs;
    metrics->total_keys = total_keys;
    metrics->revoked_certificates = revoked;
    metrics->certificates_validated = manager->certificates_validated;
    metrics->validation_failures = manager->validation_failures;

    return 0;
}
