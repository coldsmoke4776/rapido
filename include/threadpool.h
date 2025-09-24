#ifndef THREADPOOL_H
#define THREADPOOL_H
#include <pthread.h>
#include <stdbool.h>

typedef void(*job_func_t)(void*);

typedef struct job{
    job_func_t func;
    void *arg;
    struct job *next; // for chaining jobs
} job_t;

typedef struct {
    pthread_t  *threads; // Array of "worker" threads
    int thread_count; // Number of threads
    job_t *head; // Head of the job queue
    job_t *tail; // Tail of the job queue
    pthread_mutex_t lock; // Lock for the job queue, helps prevent race conditions - only one thread touches the queue at a time
    pthread_cond_t notify; // Condition variable for the job queue, helps the threads wait for new jobs.
    bool stop; // Flag to threads to stop.
} threadpool_t;

// API
int threadpool_init(threadpool_t *pool, int num_threads);
int threadpool_submit(threadpool_t *pool, job_func_t func, void *arg);
int threadpool_destroy(threadpool_t *pool);




#endif