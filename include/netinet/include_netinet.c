#include "include_netinet.h"

int include_netinet_parse_ip_header(uint8_t *packet, uint32_t size, include_netinet_header_t *header)
{
    if (!packet || !header)
    {
        return -1;
    }
    return 0;
}

int include_netinet_parse_tcp_header(uint8_t *packet, uint32_t size, include_netinet_tcp_header_t *tcp_header)
{
    if (!packet || !tcp_header)
    {
        return -1;
    }
    return 0;
}

int include_netinet_create_ip_packet(include_netinet_header_t *header, uint8_t *packet)
{
    if (!header || !packet)
    {
        return -1;
    }
    return 0;
}

int include_netinet_create_tcp_packet(include_netinet_tcp_header_t *tcp_header, uint8_t *packet)
{
    if (!tcp_header || !packet)
    {
        return -1;
    }
    return 0;
}

int include_netinet_calculate_checksum(uint8_t *data, uint32_t size, uint16_t *checksum)
{
    if (!data || !checksum)
    {
        return -1;
    }
    return 0;
}

int include_netinet_verify_checksum(uint8_t *data, uint32_t size)
{
    if (!data)
    {
        return -1;
    }
    return 0;
}
