#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/wait.h>


int main() {
    int x = 1;
    pid_t p;
    p = fork();
    if (p < 0) {
        perror("fork");
        return EXIT_FAILURE;
    }
    if (p > 0) {
        // parent process
        getppid();
        printf("parent %d, child process is %d\n", getpid(), p);
        int status;
        pid_t child = wait(&status);
        if (WIFEXITED(status)) {
            printf("wait: child %d, status %x\n", child, WEXITSTATUS(status));
        }
    } else {
        // child process
        printf("child, my pid is %d, parent is %d\n", getpid(), getppid());
        for (; x < 3; x++) {
            printf("child: x is %d\n", x);
            sleep(1);
        }
        return 3;
    }
}
