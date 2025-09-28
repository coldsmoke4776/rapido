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
        pool->head = job->next;
        if (pool->head == NULL){
            pool->tail = NULL;
        }
        pthread_mutex_unlock(&pool->lock);
        job->func(job->arg);
        free(job);
    }
    return NULL;
};


int threadpool_init(threadpool_t *pool, int num_threads) {
    // Todo: allocate, init mutex/cond, create threads
    pool->threads = malloc(num_threads * sizeof(pthread_t));
    if (pool->threads == NULL){
        fprintf(stderr, "Failed to allocate memory for threads\n");
        return -1;
    }
    pool->thread_count = num_threads;
    pool->head = NULL;
    pool->tail = NULL;
    pool->stop = false;
    if (pthread_mutex_init(&pool->lock, NULL) != 0){
        fprintf(stderr, "Failed to intiialize mutex\n");
        free(pool->threads);
        return -1;
    }
    if (pthread_cond_init(&pool->notify, NULL) != 0){
        fprintf(stderr, "Failed to initialize condition variable\n");
        pthread_mutex_destroy(&pool->lock);
        free(pool->threads);
        return -1;
    }
    for (int i=0; i < num_threads; i++){
        pthread_create(&pool->threads[i], NULL, worker_thread, pool);
    }
    return 0;
};

int threadpool_submit(threadpool_t *pool, job_func_t func, void *arg) {
    // Todo: enqueue job, signal worker thread
    job_t *job = malloc(sizeof(job_t));
    if (job == NULL){
        fprintf(stderr, "Failed to allocate memory for job\n");
        return -1;
    }
    job->func = func;
    job->arg = arg;
    job->next = NULL;
    pthread_mutex_lock(&pool->lock);
    if (pool->head == NULL){
        pool->head = job;
        pool->tail = job;
    } else {
        pool->tail->next = job;
        pool->tail =job;
    }
    pthread_cond_signal(&pool->notify);
    pthread_mutex_unlock(&pool->lock);
    return 0;
};

int threadpool_destroy(threadpool_t *pool) {
    // Todo: fset stop flag, broadcast, join threads, free resources
    pthread_mutex_lock(&pool->lock);
    pool-> stop = true;
    pthread_cond_broadcast(&pool->notify);
    pthread_mutex_unlock(&pool->lock);
    for (int i=0; i < pool->thread_count; i++){
        pthread_join(pool->threads[i], NULL);
    }
    pthread_cond_destroy(&pool->notify);
    pthread_mutex_destroy(&pool->lock);
    free(pool->threads);
    return 0;
};

