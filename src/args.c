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
#include "args.h"
#include "banner.h"

// Global variables for quiet and summary modes
int quiet_mode = 0;
int summary_mode = 0;

int parse_args(int argc, char **argv, scan_target_t *targets, size_t max_targets){
    int arg_offset = 1;

    // Handle help flag first
    if (argc > 1 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)) {
        help(argv[0]);
        exit(EXIT_SUCCESS);
    }

    // Parse optional flags (-q, -s)
    while (arg_offset < argc && argv[arg_offset][0] == '-') {
        if (strcmp(argv[arg_offset], "-q") == 0) {
            quiet_mode = 1;
        } else if (strcmp(argv[arg_offset], "-s") == 0) {
            summary_mode = 1;
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[arg_offset]);
            exit(EXIT_FAILURE);
        }
        arg_offset++;
    }

    // After flags, we expect exactly 3 arguments: target, start, end
    if (argc - arg_offset != 3) {
        fprintf(stderr, "Usage: %s [-q] [-s] <IP address|file.txt> <start_port> <end_port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *target_input = argv[arg_offset];
    int start = atoi(argv[arg_offset + 1]);
    int end   = atoi(argv[arg_offset + 2]);

    if (start < 1 || start > 65535 || end < 1 || end > 65535 || start > end) {
        fprintf(stderr, "Invalid port range: %d-%d\n", start, end);
        exit(EXIT_FAILURE);
    }

    // Case 1: Input file
    if (strstr(target_input, ".txt")) {
        return parse_file_targets(target_input, targets, max_targets, start, end);
    }

    // Case 2: CIDR notation
    if (strchr(argv[arg_offset], '/')) {
        return parse_cidr_targets(argv[arg_offset], targets, max_targets, start, end);
    }

    // Case 3: Single IP
    strncpy(targets[0].ip, target_input, sizeof(targets[0].ip) - 1);
    targets[0].ip[sizeof(targets[0].ip) - 1] = '\0';
    targets[0].start_port = start;
    targets[0].end_port   = end;

    struct in_addr tmp;
    if (inet_pton(AF_INET, targets[0].ip, &tmp) != 1) {
        fprintf(stderr, "Invalid IP address: %s\n", targets[0].ip);
        exit(EXIT_FAILURE);
    }

    return 1;
}

int parse_file_targets(const char *filename, scan_target_t *targets, size_t max_targets, int start, int end) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "Failed to open file: %s\n", filename);
        exit(EXIT_FAILURE);
    }

    char line[256];
    size_t count = 0;

    while (fgets(line, sizeof(line), fp) && count < max_targets) {
        line[strcspn(line, "\n")] = '\0';

        struct in_addr tmp;
        if (inet_pton(AF_INET, line, &tmp) != 1) {
            fprintf(stderr, "Skipping invalid IP address: %s\n", line);
            continue;
        }

        strncpy(targets[count].ip, line, sizeof(targets[count].ip) - 1);
        targets[count].ip[sizeof(targets[count].ip) - 1] = '\0';
        targets[count].start_port = start;
        targets[count].end_port   = end;
        count++;
    }

    fclose(fp);
    return count;
};


