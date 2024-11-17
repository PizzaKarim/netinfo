#include "network.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int parse_address(const char* address, Network* network)
{
    char* ptr = strchr(address, '/');
    char* ip = (char*)calloc(16, sizeof(char));
    if (ip == 0) return FAILED_ALLOCATION;
    uint8_t prefix = 0;
    if (ptr == 0) memcpy(ip, address, 15);
    else
    {
        unsigned long long pos = ptr - address;
        memcpy(ip, address, pos);
        char* p = (char*)calloc(3, sizeof(char));
        if (p == 0) return FAILED_ALLOCATION;
        memcpy(p, address + pos + 1, 2);
        int r = sscanf(p, "%hhu", &prefix);
        free(p);
        if (r != 1) return FAILED_SCAN;
    }
    uint8_t octet[4];
    int r = sscanf(ip, "%hhu.%hhu.%hhu.%hhu", &octet[3], &octet[2], &octet[1], &octet[0]);
    free(ip);
    if (r != 4) return FAILED_SCAN;
    network->ipv4 = octet[0] | (octet[1] << 8) | (octet[2] << 16) | (octet[3] << 24);
    network->class = get_network_class(network->ipv4);
    network->prefix.length = prefix != 0 ? prefix : network->class.prefix.length;
    network->prefix.mask = 0xFFFFFFFF << (32 - network->prefix.length);
    if (network->prefix.length < network->class.prefix.length) return INVALID_SUBNET;
    return 0;
}

void format_address(uint32_t ipv4, char** address)
{
    unsigned char octet1 = ipv4 >> 24;
    unsigned char octet2 = (ipv4 & 0xFF0000) >> 16;
    unsigned char octet3 = (ipv4 & 0xFF00) >> 8;
    unsigned char octet4 = ipv4 & 0xFF;
    *address = (char*)calloc(16, sizeof(char));
    snprintf(*address, 16, "%u.%u.%u.%u", octet1, octet2, octet3, octet4);
}

NetworkClass get_network_class(uint32_t ipv4)
{
    if (ipv4 >> 31 == 0b0   ) return (NetworkClass) { 'A',  8, 0xFF000000 };
    if (ipv4 >> 30 == 0b10  ) return (NetworkClass) { 'B', 16, 0xFFFF0000 };
    if (ipv4 >> 29 == 0b110 ) return (NetworkClass) { 'C', 24, 0xFFFFFF00 };
    if (ipv4 >> 28 == 0b1110) return (NetworkClass) { 'D' };
    if (ipv4 >> 28 == 0b1111) return (NetworkClass) { 'E' };
    return (NetworkClass) { '\0', 0, 0 };
}

void print_network_info(Network* network)
{
    char* address;
    format_address(network->ipv4, &address);
    printf("IPv4: %s/%hhu\n", address, network->prefix.length);
    free(address);

    printf("  Class        : %c/%hhu\n", network->class.character, network->class.prefix.length);
    
    char* network_address;
    char* broadcast_address;
    format_address(network->ipv4 & network->class.prefix.mask, &network_address);
    format_address((network->ipv4 & network->class.prefix.mask) + ~network->class.prefix.mask, &broadcast_address);
    printf("  Network      : %s\n", network_address);
    printf("  Broadcast    : %s\n", broadcast_address);
    free(network_address);
    free(broadcast_address);

    uint8_t subnet[4];
    subnet[0] = network->prefix.mask >> 24;
    subnet[1] = (network->prefix.mask & 0xFFFFFF) >> 16;
    subnet[2] = (network->prefix.mask & 0xFFFF) >> 8;
    subnet[3] = network->prefix.mask & 0xFF;
    printf("  Subnet mask  : %hhu.%hhu.%hhu.%hhu\n", subnet[0], subnet[1], subnet[2], subnet[3]);

    char* configuration = (char*)calloc(36, sizeof(char));
    if (configuration == 0) return;
    int j = 0;
    for (int i = 0; i < 32; i++)
    {
        configuration[i + j] = i < network->class.prefix.length ? 'n' : (i < network->prefix.length ? 's' : 'h');
        if (i == 7 || i == 15 || i == 23) configuration[i + ++j] = '.';
    }
    printf("  Configuration: %s\n", configuration);
    free(configuration);

    unsigned int subnets = (unsigned int)pow(2, network->prefix.length - network->class.prefix.length);
    unsigned int hosts = (unsigned int)pow(2, 32 - network->prefix.length) - 2;
    printf("  Subnets      : %u\n", subnets);
    printf("  Hosts/subnet : %u (usable)\n", hosts);
    printf("  Hosts        : %u (usable)\n", hosts * subnets);
    printf("  Subnet:\n");
    format_address(network->ipv4 & network->prefix.mask, &network_address);
    format_address((network->ipv4 & network->prefix.mask) + ((unsigned int)pow(2, 32 - network->prefix.length) - 1), &broadcast_address);
    printf("    Network    : %s\n", network_address);
    printf("    Broadcast  : %s\n", broadcast_address);
    free(network_address);
    free(broadcast_address);
}
