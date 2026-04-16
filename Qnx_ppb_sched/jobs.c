#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

void job_A(void* arg)
{
    int i = (int)(intptr_t)arg;
    printf("[A %04d] begin ", i);
    usleep(1);
    printf("end\n");
}

void job_B(void* arg)
{
    int i = (int)(intptr_t)arg;
    printf("[B %04d] begin ", i);
    usleep(1);
    printf("end\n");
}
