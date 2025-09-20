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
#include "triage.h"



/*This is going to be the "triage" engine, where we take the banner we receive 
and check the various possible interesting things a banner could give back to us and suggest next moves*/
void triage_banner(const char *banner, int port){
    if (strstr(banner, "HTTP")){
        triage_http_banner(banner, port);
    } else if (strstr(banner, "SSH")){
        triage_ssh_banner(banner, port);
    } else if (strstr(banner, "FTP")){
        triage_ftp_banner(banner, port);
    } else if (port == 22) {
        triage_ssh_banner(banner, port);
    } else if (port == 21) {
        triage_ftp_banner(banner, port);
    } else if (port == 80 || port == 443) {
        triage_http_banner(banner, port);
    } else {
        printf("[*] %d/tcp: No specific triage rules, manual follow-up needed.\n", port);
    };

};

//Copying in test code to delete later
void triage_http_banner(const char *banner, int port) {
    if (strstr(banner, "SimpleHTTP")) {
        printf("[!] %d/tcp: Python SimpleHTTP detected — check for directory listing.\n", port);
    }
    else if (strstr(banner, "Apache")) {
        printf("[!] %d/tcp: Apache detected — verify version and patch level.\n", port);
        if (strstr(banner, "2.2")){
            printf("[!] %d/tcp: Apache 2.2 detected, known to be end of life and insecure.\n", port);
        }
    }
    else if (strstr(banner, "nginx")) {
        printf("[!] %d/tcp: nginx detected — look for proxy misconfigs.\n", port);
    }
    else if (strstr(banner, "IIS")){
        printf("[!] %d/tcp: IIS detected - check for default or weak configurations.\n", port);
    } else {
        printf("[*] %d/tcp: HTTP detected, but no SimpleHTTP, Apache, nginx, or IIS banner detected, manual follow-up needed.\n", port);
    }
};

//Placeholders for other banner triage protocol engines:
void triage_ssh_banner(const char *banner, int port) {
    if (strstr(banner, "OpenSSH")){
        printf("[*] %d/tcp: OpenSSH detected - check for weak keys and default configurations.\n", port);
    } else if (strstr(banner, "7.2")){
        printf("[!] %d/tcp: SSH version 7.2 detected, known to be vulnerable to CVE-2016-0777\n", port);
    } else if (strstr(banner, "8.2")){
        printf("[!] %d/tcp: SSH version 8.2 detected, common on Ubuntu systems esp. 20.04, check for default settings.\n", port);
    } else {
        printf("[*] %d/tcp: SSH detected, but no OpenSSH banner detected, manual follow-up needed.\n ", port);
    }
};

void triage_ftp_banner(const char *banner, int port) {
    if (strstr(banner, "vsFTPd")){
        printf("[*] %d/tcp: detected %s\n", port, banner);
    } else if (strstr(banner, "2.3.4")){
        printf("[!] %d/tcp: vsFTPd 2.3.4 has a backdoor (CVE-2011-2523).\n", port);
    } else if (strstr(banner, "ProFTPd")){
        printf("[!] %d/tcp:  ProFTPD detected — check for mod_copy abuse.\n", port);
    } else {
        printf("[*] %d/tcp: FTP detected, but no vsFTPd or ProFTPd banner detected, manual follow-up needed.\n", port);
    }
};