#include "scheduler.h"
#include <stdlib.h>

static void* worker_thread(void* arg)
{
    job_queue_t* q = (job_queue_t*)arg;

    while (1) {
        pthread_mutex_lock(&q->mutex);

        while (!q->head && !q->stop) {
            pthread_cond_wait(&q->cond, &q->mutex);
        }

        if (q->stop && !q->head) {
            pthread_mutex_unlock(&q->mutex);
            break;
        }

        job_t* job = q->head;
        q->head = job->next;
        if (!q->head)
            q->tail = NULL;

        pthread_mutex_unlock(&q->mutex);

        job->fn(job->arg);
        free(job);
    }

    return NULL;
}

void scheduler_init(job_queue_t* q)
{
    q->head = q->tail = NULL;
    q->stop = 0;
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->cond, NULL);
}

void scheduler_submit(job_queue_t* q, job_fn fn, void* arg)
{
    job_t* job = (job_t*)malloc(sizeof(job_t));
    job->fn = fn;
    job->arg = arg;
    job->next = NULL;

    pthread_mutex_lock(&q->mutex);
    if (q->tail)
        q->tail->next = job;
    else
        q->head = job;
    q->tail = job;
    pthread_cond_signal(&q->cond);
    pthread_mutex_unlock(&q->mutex);
}

void scheduler_start(job_queue_t* q, int workers)
{
    for (int i = 0; i < workers; i++) {
        pthread_t t;
        pthread_create(&t, NULL, worker_thread, q);
        pthread_detach(t);
    }
}

void scheduler_shutdown(job_queue_t* q)
{
    pthread_mutex_lock(&q->mutex);
    q->stop = 1;
    pthread_cond_broadcast(&q->cond);
    pthread_mutex_unlock(&q->mutex);
}
