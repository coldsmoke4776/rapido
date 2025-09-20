#ifndef CIDR_H
#define CIDR_H
#include <arpa/inet.h>
#include <stddef.h>
#include <stdint.h>
#include "args.h"

// Generate a subnet mask from prefix length >> /24 would be 255.255.255.0
uint32_t make_mask(int prefix_len);

// Parse CIDR notation like 192.168.1.0/24 into a list of scan_target_t:
int parse_cidr_targets(const char *cidr, scan_target_t *targets, size_t max_targets, int start, int end);

#endif


