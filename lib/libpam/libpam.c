#include "libpam.h"

int libpam_init(void)
{
    return 0;
}

int libpam_start(const char *service_name, const char *username, pam_handle_t **handle)
{
    if (!service_name || !username || !handle)
    {
        return -1;
    }
    return 0;
}

int libpam_end(pam_handle_t *handle)
{
    if (!handle)
    {
        return -1;
    }
    return 0;
}

int libpam_authenticate(pam_handle_t *handle, uint32_t flags)
{
    if (!handle)
    {
        return -1;
    }
    return 0;
}

int libpam_acct_mgmt(pam_handle_t *handle, uint32_t flags)
{
    if (!handle)
    {
        return -1;
    }
    return 0;
}

int libpam_setcred(pam_handle_t *handle, uint32_t flags)
{
    if (!handle)
    {
        return -1;
    }
    return 0;
}
