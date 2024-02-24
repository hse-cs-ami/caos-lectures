#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/wait.h>


int main() {
    for (int i = 0; i < 10; ++i) {
        pid_t p;
        p = fork();
        if (p < 0) {
            perror("fork");
            return EXIT_FAILURE;
        }
        if (p > 0) {
            continue;
        }
        // child process
        printf("child, my pid is %d, parent is %d\n", getpid(), getppid());
        pid_t myself = getpid();
        for (int x = 1; x < myself % 10; x++) {
            printf("child %d: x is %d\n", getpid(), x);
            sleep(1);
        }
        return 3;
    }
    while (1) {
        if (waitpid(-1, NULL, WNOHANG) > 0) {
            puts("child reaped");
        }
        puts("parent sleeping");
        sleep(1);
    }
}
