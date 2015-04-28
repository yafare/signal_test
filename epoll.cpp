#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <signal.h>

#define EPOLL_SIZE 10
#define BACK_QUEUE 10
#define PORT 18003

const int max_count = 1000 * 10000;
volatile int cur_count = 0;

void setnonblocking(int sockFd)
{
    int opt;

    opt = fcntl(sockFd, F_GETFL);
    if (opt < 0) {
        printf("fcntl(F_GETFL) fail.");
        exit(-1);
    }
    opt |= O_NONBLOCK;
    if (fcntl(sockFd, F_SETFL, opt) < 0) {
        printf("fcntl(F_SETFL) fail.");
        exit(-1);
    }
}

timeval tv1, tv2;
void signal_handler(int signo, siginfo_t *info, void *extra)
{
    sigval val;
    if (signo == SIGRTMIN) {
        if (cur_count > max_count) {
            return;
        }

        if (cur_count == 0) {
            gettimeofday(&tv1, 0);
        } else if (cur_count == max_count) {
            gettimeofday(&tv2, 0);
            double timeuse = 1000000*(tv2.tv_sec - tv1.tv_sec) + tv2.tv_usec - tv1.tv_usec;
            printf("over, cur_count = %d, elapsed = %f\n", cur_count, timeuse);
        }
        cur_count++;
        sigqueue(getpid(), SIGRTMIN, val);
    }
}

int main()
{
    int serverFd = socket(AF_INET, SOCK_STREAM, 0);

    setnonblocking(serverFd);

    int epFd = epoll_create(EPOLL_SIZE);
    struct epoll_event ev, evs[1];
    ev.data.fd = serverFd;
    ev.events = EPOLLIN | EPOLLET;
    epoll_ctl(epFd, EPOLL_CTL_ADD, serverFd, &ev);

    struct sockaddr_in serverAddr;
    socklen_t serverLen = sizeof(struct sockaddr_in);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(PORT);
    if (bind(serverFd, (struct sockaddr *)&serverAddr, serverLen)) {
        printf("bind() fail: %d.\n", errno);
        exit(-1);
    }

    if (listen(serverFd, BACK_QUEUE)) {
        printf("Listen fail.\n");
        exit(-1);
    }

    struct sigaction action;
    action.sa_flags = SA_SIGINFO; 
    action.sa_sigaction = &signal_handler;
    if (sigaction(SIGRTMIN, &action, 0) == -1) { 
        printf("sigusr: sigaction\n");
        return 1;
    }

    sigset_t sig_set;
    sigfillset(&sig_set);
    sigdelset(&sig_set, SIGRTMIN);
    while (1) {
        int nfds = epoll_pwait(epFd, evs, 1, -1, &sig_set);
    }
}
