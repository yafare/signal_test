#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>

void Usage(const char *name)
{
    printf("%s pid\n", name);
}

const int max_count = 10 * 10000;
volatile int cur_count = 0;

void signal_handler(int sig)
{
    if (sig == SIGRTMIN) {
        cur_count++;
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        Usage(argv[0]);
        return 0;
    }

    signal(SIGRTMIN, signal_handler);

    int pid = atoi(argv[1]);
    //int pid = getpid();
    sigval val;
    val.sival_int = (int)getpid();

    timeval tv1, tv2;
    gettimeofday(&tv1, 0);

    sigqueue(pid, SIGRTMIN, val);
    //for (int i = 0; i < max_count; i++) {
    //    sigqueue(pid, SIGRTMIN, val);
    //}
    //while (cur_count < max_count) {
    //    ;
    //}

    gettimeofday(&tv2, 0);
    double timeuse = 1000000*(tv2.tv_sec - tv1.tv_sec) + tv2.tv_usec - tv1.tv_usec;
    printf("over, cur_count = %d, elapsed = %f\n", cur_count, timeuse);
    getchar();
}
