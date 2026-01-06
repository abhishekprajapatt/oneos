#ifndef LIBNSL_H
#define LIBNSL_H

#include <stdint.h>

int libnsl_init(void);
int libnsl_getservbyname(const char *name, const char *proto, void **serv_entry);
int libnsl_getprotobyname(const char *name, void **proto_entry);
int libnsl_gethostbyname(const char *name, void **host_entry);
int libnsl_getnetbyname(const char *name, void **net_entry);

#endif
