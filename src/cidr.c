// Core C Headers (In most C files, or wouldn't hurt to include)
#include <stdio.h> // Standard I/O , for printf, fprintf, stderr etc.
#include <stdlib.h> // Standard Library, for malloc(), free(), atoi, exit etc.
#include <string.h> // String and Memory Ops Library, for strcpy, strstr, strlen, memset etc.
#include <unistd.h> // Access to the OS, for close, read, write, sleep, getopt etc.
#include <errno.h> // Exposes the global errno set, so you can find out WHY a syscall failed.

// Networking-related headers (needed for most of the functionality of a network scanner)
#include <sys/types.h> // Provides general POSIX system types some socket calls rely on. (size_t, ssize_t, off_t)
#include <sys/socket.h> // Provides core socket API, without this you get no TCP functionality at all (socket, connect, bind, recv etc.)
#include <netinet/in.h> // Provides definitions for Internet address constants, which will be used in network probing.
#include <arpa/inet.h> // Provides the functionality for turning IP addresses into the binary that the socket API expects.

//Bonus/Interesting Headers
// These are not strictly necessary for the functionality of the program, but they are interesting and useful.
#include <sys/time.h> // Provides the functionality for implementing timeouts.
#include "cidr.h"

// Generate a subnet mask from a prefix length (e.g. /24 -> 255.255.255.0)
uint32_t make_mask(int prefix_len) {
    if (prefix_len < 0 || prefix_len > 32) {
        return 0; // invalid prefix
    }

    // Start with all 1s, then shift left to clear host bits.
    uint32_t mask = 0xFFFFFFFF;
    mask = mask << (32 - prefix_len);

    return mask; // <-- return mask in host byte order
}


// Parse CIDR notation like 192.168.1.0/24 into a list of scan_target_t:
int parse_cidr_targets(const char *cidr, scan_target_t *targets, size_t max_targets, int start, int end){
    printf("[DEBUG] Expanding CIDR: %s into targets...\n", cidr);
    char ip_str[64]; // Buffer to store the IP address
    int prefix_len; // Prefix length /24, /32 etc.

    //We need to split the input up into "IP" and "prefix" sections to deal with them separately.
    if (sscanf(cidr, "%63[^/]/%d", ip_str, &prefix_len) != 2){
        fprintf(stderr, "Invalid CIDR notation: %s\n", cidr);
        return 0;
    };

    printf("[DEBUG] Base IP: %s, Prefix length: %d\n", ip_str, prefix_len);

    //We also need to account for an invalid prefix length that wouldn't make sense for an IPv4 address
    if (prefix_len <- 0 || prefix_len > 32){
        fprintf(stderr, "Invalid prefix length: %d\n", prefix_len);
        return 0;
    };

    // We need to check whether the IP part of the CIDR input matches the proper format.
    struct in_addr base_ip;
    if (inet_pton(AF_INET, ip_str, &base_ip) != 1){
        fprintf(stderr, "Invalid IP address: %s\n", ip_str);
        return 0;
    };

    // Convert everything to host order for math
    uint32_t mask = make_mask(prefix_len);
    uint32_t ip = ntohl(base_ip.s_addr);

    uint32_t network   = ip & mask;
    uint32_t broadcast = network | ~mask;

    // Debug print the calculated network/broadcast
    char net_str[INET_ADDRSTRLEN];
    char bcast_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(struct in_addr){ htonl(network) }, net_str, sizeof(net_str));
    inet_ntop(AF_INET, &(struct in_addr){ htonl(broadcast) }, bcast_str, sizeof(bcast_str));
    printf("[DEBUG] Network: %s, Broadcast: %s\n", net_str, bcast_str);

    // Expand range into targets[]
    int count = 0;
    for (uint32_t addr = network + 1; addr < broadcast && count < (int)max_targets; addr++) {
        struct in_addr target_ip;
        target_ip.s_addr = htonl(addr); // back to network order for inet_ntop

        if (!inet_ntop(AF_INET, &target_ip, targets[count].ip, sizeof(targets[count].ip))) {
            fprintf(stderr, "Failed to convert IP back to string\n");
            continue;
        }

        targets[count].start_port = start;
        targets[count].end_port   = end;

        printf("[DEBUG] Added target: %s (ports %d-%d)\n",
                targets[count].ip, targets[count].start_port, targets[count].end_port);

        count++;
    }

    return count;
}