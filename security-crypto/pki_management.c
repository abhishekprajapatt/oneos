#include "pki_management.h"
#include <stdlib.h>
#include <string.h>

static PKIManager *global_pki = NULL;
static uint32_t next_public_key_id = 1;
static uint32_t next_private_key_id = 1;
static uint32_t next_key_pair_id = 1;
static uint32_t next_chain_id = 1;

PKIManager *pki_manager_init(void)
{
    PKIManager *manager = (PKIManager *)malloc(sizeof(PKIManager));
    if (!manager)
        return NULL;

    memset(manager, 0, sizeof(PKIManager));
    manager->pki_manager_id = 1;
    manager->public_key_count = 0;
    manager->private_key_count = 0;
    manager->key_pair_count = 0;
    manager->chain_count = 0;

    global_pki = manager;
    return manager;
}

int pki_manager_destroy(PKIManager *manager)
{
    if (!manager)
        return -1;

    for (uint32_t i = 0; i < manager->private_key_count; i++)
    {
        memset(&manager->private_keys[i].key_data, 0, sizeof(manager->private_keys[i].key_data));
    }

    free(manager);
    global_pki = NULL;
    return 0;
}

KeyPair *pki_generate_key_pair(PKIManager *manager, KeyType type, uint16_t key_size)
{
    if (!manager || manager->key_pair_count >= MAX_KEY_PAIRS)
        return NULL;

    if (manager->public_key_count >= MAX_PUBLIC_KEYS || manager->private_key_count >= MAX_PRIVATE_KEYS)
        return NULL;

    PublicKey *pub_key = &manager->public_keys[manager->public_key_count];
    memset(pub_key, 0, sizeof(PublicKey));
    pub_key->public_key_id = next_public_key_id++;
    pub_key->key_type = type;
    pub_key->key_size = key_size;
    pub_key->created_timestamp = 0;

    for (uint16_t i = 0; i < key_size && i < 512; i++)
    {
        pub_key->key_data[i] = (uint8_t)((i * 11 + 23) % 256);
    }

    manager->public_key_count++;

    PrivateKey *priv_key = &manager->private_keys[manager->private_key_count];
    memset(priv_key, 0, sizeof(PrivateKey));
    priv_key->private_key_id = next_private_key_id++;
    priv_key->key_type = type;
    priv_key->key_size = key_size;
    priv_key->created_timestamp = 0;
    priv_key->is_encrypted = 0;

    for (uint16_t i = 0; i < key_size && i < 1024; i++)
    {
        priv_key->key_data[i] = (uint8_t)((i * 13 + 31) % 256);
    }

    manager->private_key_count++;

    KeyPair *pair = &manager->key_pairs[manager->key_pair_count];
    memset(pair, 0, sizeof(KeyPair));
    pair->key_pair_id = next_key_pair_id++;
    pair->public_key = pub_key;
    pair->private_key = priv_key;
    pair->created_timestamp = 0;
    pair->expires_timestamp = 0;

    manager->key_pair_count++;
    return pair;
}

int pki_import_public_key(PKIManager *manager, const uint8_t *key_data, uint16_t key_size)
{
    if (!manager || !key_data || key_size == 0 || key_size > 512)
        return -1;

    if (manager->public_key_count >= MAX_PUBLIC_KEYS)
        return -1;

    PublicKey *pub_key = &manager->public_keys[manager->public_key_count];
    memset(pub_key, 0, sizeof(PublicKey));
    pub_key->public_key_id = next_public_key_id++;
    pub_key->key_size = key_size;
    pub_key->created_timestamp = 0;
    memcpy(pub_key->key_data, key_data, key_size);

    manager->public_key_count++;
    return 0;
}

int pki_import_private_key(PKIManager *manager, const uint8_t *key_data, uint16_t key_size)
{
    if (!manager || !key_data || key_size == 0 || key_size > 1024)
        return -1;

    if (manager->private_key_count >= MAX_PRIVATE_KEYS)
        return -1;

    PrivateKey *priv_key = &manager->private_keys[manager->private_key_count];
    memset(priv_key, 0, sizeof(PrivateKey));
    priv_key->private_key_id = next_private_key_id++;
    priv_key->key_size = key_size;
    priv_key->created_timestamp = 0;
    priv_key->is_encrypted = 0;
    memcpy(priv_key->key_data, key_data, key_size);

    manager->private_key_count++;
    return 0;
}

int pki_sign_data(PKIManager *manager, uint32_t private_key_id, const uint8_t *data, uint64_t data_len, uint8_t *signature)
{
    if (!manager || !data || !signature || data_len == 0)
        return -1;

    for (uint32_t i = 0; i < manager->private_key_count; i++)
    {
        if (manager->private_keys[i].private_key_id == private_key_id)
        {
            uint32_t sig_val = 0;
            for (uint64_t j = 0; j < data_len; j++)
            {
                sig_val = ((sig_val << 5) + sig_val) + data[j];
            }
            for (uint16_t j = 0; j < manager->private_keys[i].key_size; j++)
            {
                sig_val ^= manager->private_keys[i].key_data[j];
            }
            memcpy(signature, &sig_val, 4);
            return 0;
        }
    }

    return -1;
}

int pki_verify_signature(PKIManager *manager, uint32_t public_key_id, const uint8_t *data, uint64_t data_len, const uint8_t *signature)
{
    if (!manager || !data || !signature || data_len == 0)
        return -1;

    for (uint32_t i = 0; i < manager->public_key_count; i++)
    {
        if (manager->public_keys[i].public_key_id == public_key_id)
        {
            uint32_t expected_sig = 0;
            for (uint64_t j = 0; j < data_len; j++)
            {
                expected_sig = ((expected_sig << 5) + expected_sig) + data[j];
            }
            for (uint16_t j = 0; j < manager->public_keys[i].key_size; j++)
            {
                expected_sig ^= manager->public_keys[i].key_data[j];
            }

            uint32_t actual_sig;
            memcpy(&actual_sig, signature, 4);

            return (expected_sig == actual_sig) ? 0 : -1;
        }
    }

    return -1;
}

CertificateChain *pki_create_certificate_chain(PKIManager *manager, uint32_t key_pair_id)
{
    if (!manager || manager->chain_count >= MAX_CERTIFICATE_CHAINS)
        return NULL;

    for (uint32_t i = 0; i < manager->key_pair_count; i++)
    {
        if (manager->key_pairs[i].key_pair_id == key_pair_id)
        {
            CertificateChain *chain = &manager->chains[manager->chain_count];
            memset(chain, 0, sizeof(CertificateChain));
            chain->chain_id = next_chain_id++;
            chain->key_pair = &manager->key_pairs[i];
            chain->chain_length = 0;
            chain->is_trusted = 0;

            manager->chain_count++;
            return chain;
        }
    }

    return NULL;
}

int pki_add_certificate_to_chain(PKIManager *manager, uint32_t chain_id, const uint8_t *cert_data)
{
    if (!manager || !cert_data)
        return -1;

    for (uint32_t i = 0; i < manager->chain_count; i++)
    {
        if (manager->chains[i].chain_id == chain_id)
        {
            if (manager->chains[i].chain_length >= 16)
                return -1;

            manager->chains[i].certificate_chain[manager->chains[i].chain_length] = (uint8_t *)malloc(256);
            if (!manager->chains[i].certificate_chain[manager->chains[i].chain_length])
                return -1;

            memcpy(manager->chains[i].certificate_chain[manager->chains[i].chain_length], cert_data, 256);
            manager->chains[i].chain_length++;
            return 0;
        }
    }

    return -1;
}

int pki_validate_certificate_chain(PKIManager *manager, uint32_t chain_id)
{
    if (!manager)
        return -1;

    for (uint32_t i = 0; i < manager->chain_count; i++)
    {
        if (manager->chains[i].chain_id == chain_id)
        {
            if (manager->chains[i].chain_length > 0)
            {
                manager->chains[i].is_trusted = 1;
            }
            return 0;
        }
    }

    return -1;
}

int pki_export_public_key(PKIManager *manager, uint32_t public_key_id, uint8_t *key_data)
{
    if (!manager || !key_data)
        return -1;

    for (uint32_t i = 0; i < manager->public_key_count; i++)
    {
        if (manager->public_keys[i].public_key_id == public_key_id)
        {
            memcpy(key_data, manager->public_keys[i].key_data, manager->public_keys[i].key_size);
            return 0;
        }
    }

    return -1;
}

int pki_export_private_key(PKIManager *manager, uint32_t private_key_id, uint8_t *key_data)
{
    if (!manager || !key_data)
        return -1;

    for (uint32_t i = 0; i < manager->private_key_count; i++)
    {
        if (manager->private_keys[i].private_key_id == private_key_id)
        {
            memcpy(key_data, manager->private_keys[i].key_data, manager->private_keys[i].key_size);
            return 0;
        }
    }

    return -1;
}
