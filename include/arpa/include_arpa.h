#ifndef INCLUDE_ARPA_H
#define INCLUDE_ARPA_H

#include <stdint.h>

typedef struct
{
    uint32_t addr;
} include_arpa_addr_t;

typedef struct
{
    uint16_t port;
    uint32_t addr;
} include_arpa_sockaddr_t;

uint32_t include_arpa_inet_addr(const char *addr_str);
const char *include_arpa_inet_ntoa(uint32_t addr);
uint16_t include_arpa_htons(uint16_t hostshort);
uint16_t include_arpa_ntohs(uint16_t netshort);
uint32_t include_arpa_htonl(uint32_t hostlong);
uint32_t include_arpa_ntohl(uint32_t netlong);

#endif
