#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main() {
    int pipefd[2];

    if (pipe(pipefd) < 0) {
        perror("pipe");
        return EXIT_FAILURE;
    }

    if (fork()) {
        close(pipefd[0]);
        for (int i = 0; i < 3; ++i) {
            sleep(1);
            write(pipefd[1], "abc\n", 4);
        }
    } else {
        char buf[1024];
        ssize_t size;
        close(pipefd[1]);
        while (1) {
            if ((size = read(pipefd[0], buf, sizeof(buf))) <= 0) {
                exit(EXIT_SUCCESS);
            }
            write(STDOUT_FILENO, buf, size);
        }
        exit(EXIT_SUCCESS);
    }
}
