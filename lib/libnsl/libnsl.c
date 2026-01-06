#include "libnsl.h"

int libnsl_init(void)
{
    return 0;
}

int libnsl_getservbyname(const char *name, const char *proto, void **serv_entry)
{
    if (!name || !proto || !serv_entry)
    {
        return -1;
    }
    return 0;
}

int libnsl_getprotobyname(const char *name, void **proto_entry)
{
    if (!name || !proto_entry)
    {
        return -1;
    }
    return 0;
}

int libnsl_gethostbyname(const char *name, void **host_entry)
{
    if (!name || !host_entry)
    {
        return -1;
    }
    return 0;
}

int libnsl_getnetbyname(const char *name, void **net_entry)
{
    if (!name || !net_entry)
    {
        return -1;
    }
    return 0;
}
