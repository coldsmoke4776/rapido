#ifndef ARGS_H
#define ARGS_H

#include <stddef.h>   // for size_t

typedef struct {
    char ip[64];
    int start_port;
    int end_port;
} scan_target_t;

int parse_args(int argc, char **argv, scan_target_t *targets, size_t max_targets);
int parse_file_targets(const char *filename, scan_target_t *targets, size_t max_targets, int start, int end);
int parse_cidr_targets(const char *cidr, scan_target_t *targets, size_t max_targets, int start, int end);

extern int quiet_mode;

#endif
