#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

void signal_handler(int signo, siginfo_t *info, void *extra)
{
    sigval val;
    if (signo == SIGRTMIN) {
        sigqueue(info->si_pid, SIGRTMIN, val);
    }
}

int main()
{
    struct sigaction action;
    action.sa_flags = SA_SIGINFO; 
    action.sa_sigaction = &signal_handler;
    if (sigaction(SIGRTMIN, &action, 0) == -1) { 
        printf("sigusr: sigaction\n");
        return 1;
    }

    while (1) {
        ;
    }
}
