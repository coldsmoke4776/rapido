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
#include "banner.h"
#include "args.h"
#include "probe.h"
#include "triage.h"
#include "colors.h"
#include "cidr.h"

#define MAX_TARGETS 1024 // Maximum number of targets to scan at once, to avoid dynamic allocation problems.

int main(int argc, char **argv) {
    banner();

    scan_target_t targets[MAX_TARGETS];
    int target_count = parse_args(argc, argv, targets, MAX_TARGETS);

    for (int i = 0; i < target_count; i++) {
        printf(COLOR_CYAN "========== Scanning %s from port %d to port %d ==========\n" COLOR_RESET, targets[i].ip, targets[i].start_port, targets[i].end_port);
        int open_count = 0;
        int closed_count = 0;
        for (int port = targets[i].start_port; port <= targets[i].end_port; port++) {
            int status = probe_port_ipv4(targets[i].ip, port, 100);
            if (status == 1) {
                open_count++;
            } else {
                closed_count++;
            }
        }
        printf(COLOR_CYAN "--------------Done scanning %s------------------\n" COLOR_RESET, targets[i].ip);
        printf(COLOR_YELLOW "[Summary] %s: %d open, %d closed\n" COLOR_RESET,
            targets[i].ip, open_count, closed_count);
    };

    return 0;
}
