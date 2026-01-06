#ifndef LIBPAM_H
#define LIBPAM_H

#include <stdint.h>

typedef void pam_handle_t;

int libpam_init(void);
int libpam_start(const char *service_name, const char *username, pam_handle_t **handle);
int libpam_end(pam_handle_t *handle);
int libpam_authenticate(pam_handle_t *handle, uint32_t flags);
int libpam_acct_mgmt(pam_handle_t *handle, uint32_t flags);
int libpam_setcred(pam_handle_t *handle, uint32_t flags);

#endif
