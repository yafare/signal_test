#include <stdio.h>
#include <time.h>
#include <sys/time.h>

int max_count = 1 * 10000;

int main()
{
    timeval tv1, tv2;
    gettimeofday(&tv1, 0);

    timespec tq = { 0 }, tm = { 0 };
    tq.tv_nsec = 1;
    for (int i = 0; i < max_count; i++) {
        nanosleep(&tq, &tm);
    }

    gettimeofday(&tv2, 0);
    double timeuse = 1000000*(tv2.tv_sec - tv1.tv_sec) + tv2.tv_usec - tv1.tv_usec;
    printf("over, cur_count = %d, elapsed = %f\n", max_count, timeuse);
    getchar();
}
