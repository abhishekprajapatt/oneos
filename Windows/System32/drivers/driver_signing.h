#ifndef DRIVER_SIGNING_H
#define DRIVER_SIGNING_H

#include <stdint.h>

int driver_signing_init(void);
int driver_signing_sign_driver(const char *driver_file, const char *cert_file);
int driver_signing_verify_driver(const char *driver_file);
int driver_signing_get_signature(const char *driver_file, char *signature, uint32_t max_len);
int driver_signing_enforce_policy(uint32_t policy_level);
int driver_signing_check_enforcement(void);

#endif
