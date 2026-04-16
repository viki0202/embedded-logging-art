#pragma once
#include <pthread.h>

typedef void (*job_fn)(void*);

typedef struct job {
    job_fn fn;
    void* arg;
    struct job* next;
} job_t;

typedef struct {
    job_t* head;
    job_t* tail;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int stop;
} job_queue_t;

void scheduler_init(job_queue_t* q);
void scheduler_submit(job_queue_t* q, job_fn fn, void* arg);
void scheduler_start(job_queue_t* q, int workers);
void scheduler_shutdown(job_queue_t* q);
