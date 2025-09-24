#include "threadpool.h"
#include <stdio.h>    // I/O functions (printf, etc.)
#include <stdlib.h>   // Memory management (malloc, exit, etc.)  
#include <string.h>   // String manipulation
#include <unistd.h>   // POSIX system calls
#include <errno.h>    // Error handling
#include <stdbool.h>  // Boolean type (C99+)

// Todo: Worker thread loop


int threadpool_init(threadpool_t *pool, int num_threads) {
    // Todo: allocate, init mutex/cond, create threads
    return 0;
};

int threadpool_submit(threadpool_t *pool, job_func_t func, void *arg) {
    // Todo: enqueue job, signal worker thread
    return 0;
};

int threadpool_destroy(threadpool_t *pool) {
    // Todo: fset stop flag, broadcast, join threads, free resources
    return 0;
}