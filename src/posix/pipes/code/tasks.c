#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

void handler(int sig) {
    write(STDOUT_FILENO, "reaping child\n", strlen("reaping child\n"));
    wait(NULL);
}

int main() {
    pid_t child;

    int fds[2];
    pipe(fds);

    signal(SIGCHLD, handler);

    for (int i = 0; i < 4; ++i) {
        if (!fork()) {
            close(fds[1]);
            int task;
            // STDIN_FILENO=0, STDOUT_FILENO=1
            while (read(fds[0], &task, sizeof(task)) > 0) {
                printf("pid %d: sleeping for %d seconds\n", getpid(), task);
                sleep(task);
            }
            return EXIT_SUCCESS;
        }
    }
    for (int i = 1; i < 10; ++i) {
        int task = i % 3 + 1;
        write(fds[1], &task, sizeof(task));
    }
    close(fds[1]);
    while (waitpid(-1, NULL, 0) != -1);
}