#ifndef INCLUDE_NETINET_H
#define INCLUDE_NETINET_H

#include <stdint.h>

typedef struct
{
    uint8_t src_addr[4];
    uint8_t dst_addr[4];
    uint8_t protocol;
    uint16_t flags;
} include_netinet_header_t;

typedef struct
{
    uint16_t src_port;
    uint16_t dst_port;
    uint32_t seq_num;
    uint32_t ack_num;
} include_netinet_tcp_header_t;

int include_netinet_parse_ip_header(uint8_t *packet, uint32_t size, include_netinet_header_t *header);
int include_netinet_parse_tcp_header(uint8_t *packet, uint32_t size, include_netinet_tcp_header_t *tcp_header);
int include_netinet_create_ip_packet(include_netinet_header_t *header, uint8_t *packet);
int include_netinet_create_tcp_packet(include_netinet_tcp_header_t *tcp_header, uint8_t *packet);
int include_netinet_calculate_checksum(uint8_t *data, uint32_t size, uint16_t *checksum);
int include_netinet_verify_checksum(uint8_t *data, uint32_t size);

#endif
