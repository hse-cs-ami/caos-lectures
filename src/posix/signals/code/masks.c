#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

volatile sig_atomic_t counter = 0;

void sayhi(int signo) {
    if (++counter >= 3) {
        exit(1);
    }
    write(STDOUT_FILENO, "hi guys\n", 8);
}

int main() {
    char c;
    struct sigaction sa = {
        .sa_handler = sayhi,
        .sa_flags = 0,
    };
    sigaction(SIGALRM, &sa, NULL);
    char buf[1024];
    ssize_t n;
    alarm(3);

    sigset_t mask, oldmask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    if (sigprocmask(SIG_BLOCK, &mask, &oldmask) < 0) {
        perror("sigprocmask");
    }
    while ((n = read(STDIN_FILENO, buf, sizeof(buf))) > 0) {
        write(STDOUT_FILENO, buf, n);
    }
    sigsuspend(&oldmask);
    sigprocmask(SIG_SETMASK, &oldmask, NULL);
    printf("read() returned a value <= 0\n");
}
