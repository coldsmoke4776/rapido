#include "threadpool.h"
#include <stdio.h>    // I/O functions (printf, etc.)
#include <stdlib.h>   // Memory management (malloc, exit, etc.)  
#include <string.h>   // String manipulation
#include <unistd.h>   // POSIX system calls
#include <errno.h>    // Error handling
#include <stdbool.h>  // Boolean type (C99+)

// Todo: Worker thread loop
static void *worker_thread(void *arg) {
    threadpool_t *pool = (threadpool_t *)arg;
    while (true) {
        pthread_mutex_lock(&pool->lock);
        while (pool->head == NULL && !pool->stop){
            pthread_cond_wait(&pool->notify, &pool->lock);
        }
        if(pool->stop && pool->head == NULL){
            pthread_mutex_unlock(&pool->lock);
            break;
        }
        // TODO: dequeue a job here, unlock, run it
        job_t *job = pool->head;
        pool-> head = job->next;
        pthread_mutex_unlock(&pool->lock);
        job->func(job->arg);
        free(job);
    }
    return NULL;
}


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
};