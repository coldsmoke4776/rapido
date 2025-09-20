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

// Prints the banner of the program, mostly decorative.
void banner() {
    printf("==============================================\n\n");
    printf("            MATTasploit Framework (TM)            \n");
    printf("==============================================\n\n");
    printf("            Rapido Reconnaissance Tool              \n");
    printf("==============================================\n\n");
};

// Prints usage info. Pass argv[0] from main for the program name.
void help(const char *prog) {
    printf("Usage: %s <IP address> <start_port> <end_port>\n", prog);
    printf("Options:\n");
    printf("  -h, --help       Show this help message and exit\n");
    printf("  --md <file>      Write Markdown report to <file>\n");
    printf("  --timeout <ms>   Connect/read timeout (default 600ms)\n");
};