// Core C Headers (In most C files, or wouldn't hurt to include)
#include <stdio.h> // Standard I/O , for printf, fprintf, stderr etc.
#include <stdlib.h> // Standard Library, for malloc(), free(), atoi, exit etc.
#include <string.h> // String and Memory Ops Library, for strcpy, strstr, strlen, memset etc.
#include <unistd.h> // Access to the OS, for close, read, write, sleep, getopt etc.
#include <errno.h> // Exposes the global errno set, so you can find out WHY a syscall failed.
#include <pthread.h> // Exposes pthread API, for multithreading

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
#include "threadpool.h"

#define MAX_TARGETS 1024 // Maximum number of targets to scan at once, to avoid dynamic allocation problems.

typedef struct {
    const char *ip;
    int port;
    int timeout_ms;
    int *open_count;
    int *closed_count;
    pthread_mutex_t *cnt_lock; // lock to protect counters
} probe_args_t;

void print_job(void *arg){
    int id = *(int*)arg;
    printf("===== CONCURRENCY CHECK =====\n");
    printf("Job %d executed (thread %lu)\n", id, (unsigned long)pthread_self());
    free(arg); // free the memory passed in
}

void probe_job(void *varg){
    probe_args_t *pa = (probe_args_t*)varg;

    int status = probe_port_ipv4(pa->ip, pa->port, pa->timeout_ms);

    if(status ==1) {
        printf("[*] TCP - Port %d is OPEN on %s\n", pa->port, pa->ip);
    } //else {
    //    printf("[x] TCP - Port %d is CLOSED on %s\n", pa->port, pa->ip);
   // }

    //Update counters safely
    pthread_mutex_lock(pa->cnt_lock);
    if (status == 1){
        (*pa->open_count)++;
    } else {
        (*pa->closed_count)++;
    }
    pthread_mutex_unlock(pa->cnt_lock);
    free(pa); // Free the pa struct, NOT ip itself
}

int main(int argc, char **argv) {
    banner();

    scan_target_t targets[MAX_TARGETS];
    int target_count = parse_args(argc, argv, targets, MAX_TARGETS);

    /*Threadpool test harness*/
    threadpool_t pool;
    int num_workers = 4;
    if(threadpool_init(&pool, num_workers) != 0) {
        fprintf(stderr, "Failed to init threadpool\n");
        return 1;
    }

    /* submit 10 print jobs to validate pool */
    for (int i =0; i < 10; i++){
        int *id = malloc(sizeof(int));
        *id = i;
        threadpool_submit(&pool, print_job, id); 
    }

    /* wait for jobs to shut down gracefully */
    threadpool_destroy(&pool);


    for (int i = 0; i < target_count; i++) {
        printf(COLOR_CYAN "========== Scanning %s from port %d to port %d ==========\n" COLOR_RESET, targets[i].ip, targets[i].start_port, targets[i].end_port);
        int open_count = 0;
        int closed_count = 0;

        pthread_mutex_t cnt_lock;
        pthread_mutex_init(&cnt_lock, NULL);

        threadpool_t pool;
        int num_workers = 20;
        threadpool_init(&pool, num_workers);

        for (int port = targets[i].start_port; port <= targets[i].end_port; port++) {
            probe_args_t *pa = malloc(sizeof(*pa));
            pa->ip = targets[i].ip; //safe, lives through this process
            pa->port = port;
            pa->timeout_ms = 200;
            pa->open_count = &open_count;
            pa->closed_count = &closed_count;
            pa->cnt_lock = &cnt_lock;

            threadpool_submit(&pool, probe_job, pa);
        }

        threadpool_destroy(&pool);
       
        printf(COLOR_CYAN "--------------Done scanning %s------------------\n" COLOR_RESET, targets[i].ip);
        printf(COLOR_YELLOW "[Summary] %s: %d open, %d closed\n" COLOR_RESET,
            targets[i].ip, open_count, closed_count);
        
            pthread_mutex_destroy(&cnt_lock);
    };

    return 0;
}
