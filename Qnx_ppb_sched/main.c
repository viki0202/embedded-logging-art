#include "scheduler.h"
#include <stdint.h>
#include <unistd.h>

void job_A(void*);
void job_B(void*);

int main(void)
{
    job_queue_t q;
    scheduler_init(&q);

    scheduler_start(&q, 2); // dwa workery

    for (int i = 0; i < 2000; i++) {
        scheduler_submit(&q, job_A, (void*)(intptr_t)i);
        scheduler_submit(&q, job_B, (void*)(intptr_t)i);
    }

    sleep(1);
    scheduler_shutdown(&q);
    sleep(1);

    return 0;
}
