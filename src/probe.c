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
#include "probe.h"
#include "triage.h"
#include "colors.h"
#include "args.h"

extern int quiet_mode; // bringing in global flag from args.c


// Given a validated port, check if the port is open and let is know.
// Return 1=open, 0=closed, -1=error (bad args, etc.)
int probe_port_ipv4(const char *ip, int port, int timeout_in_ms){
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        fprintf(stderr, "Failed to create socket: %s\n", strerror(errno));
        return -1;
    };

    struct sockaddr_in sa; // Socket address, internet, sa is naming convention for "socket address"
    sa.sin_family = AF_INET; // socket address, short internet address to get the family to Ipv4
    sa.sin_port = htons(port); // socket address, short internet address to get the port to the network byte order
    inet_pton(AF_INET, ip, &sa.sin_addr); // socket address, convirting ASCII text IP address in char *ip to binary format

    // Configure timeout for connect()/recv()
    struct timeval tv;
    tv.tv_sec  = timeout_in_ms / 1000;              // whole seconds
    tv.tv_usec = (timeout_in_ms % 1000) * 1000;     // leftover microseconds

    if (setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) < 0) {
        perror("setsockopt(SO_SNDTIMEO) failed");
    };

    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        perror("setsockopt(SO_RCVTIMEO) failed");
    };


    int result = connect(sockfd, (struct sockaddr *)&sa, sizeof(sa)); // We need to store the result of a connection to return a result

    if (result == 0) {
        //Connect is a success, port is open
        printf("Port %d is open on %s\n", port, ip);
        char buffer[1024];
        memset(buffer, 0, sizeof(buffer)); // Set the memory up and clear the buffer by filling with 0s, safe to print and inspect in memory.
        //Active banner grabbing requires us to "poke" quiet services like HTTP to give us a response for recv to recieve.
        const char *http_probe = "HEAD / HTTP/1.0\r\n\r\n";
        send(sockfd, http_probe, strlen(http_probe), 0); // send is a function from the socket API that sends data to the socket
        // recv is a function from the socket API that recives data, we're using this for banner grabbing functionality.
        int bytes = recv(sockfd, buffer, sizeof(buffer) -1, 0); // Receive the data from the socket and store it in the buffer
        // bytes is the number of bytes received from the socket
        if (bytes > 0) {
            if (!quiet_mode){
                printf(COLOR_GREEN "Port %d is open on %s " COLOR_RESET "Banner: %s\n", port, ip, buffer);
            } else {
                // Quiet mode just prints essentials
                printf(COLOR_GREEN "%s:%d OPEN" COLOR_RESET "\n", ip, port);
            }
            triage_banner(buffer, port);
        } else if (bytes == 0){
            if (!quiet_mode) {
                printf(COLOR_GREEN "Port %d is open on %s " COLOR_RESET "but no banner received\n", port, ip);
            } else {
                // Quiet mode just prints essentials
                printf(COLOR_GREEN "%s:%d OPEN" COLOR_RESET "\n", ip, port);
            }
        } else {
            perror("recv failed");
        };
        close(sockfd);
        return 1;
        } else {
            //Connect is a failure, port is closed
            if (!quiet_mode) {
                printf(COLOR_RED "[-] %d/tcp CLOSED on %s\n" COLOR_RESET, port, ip);
            };
            close(sockfd);
            return 0;
        };
};