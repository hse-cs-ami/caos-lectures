#include "listener.h"
#include <stdio.h>
#include <sys/wait.h>


int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s SERVICE\n", argv[0]);
        return 1;
    }
    int sock = create_listener(argv[1]);
    if (sock < 0) {
        return 1;
    }

    while (1) {
        int connection = accept(sock, NULL, NULL);
        if (fork() == 0) {
            // worker
            close(sock);  // we won't be accepting anything here
            char buf[1024];
            ssize_t s;
            while ((s = read(connection, buf, sizeof(buf))) > 0) {
                write(connection, buf, s);
            }
            return 0;
        } else {
            close(connection);
            while (waitpid(-1, NULL, WNOHANG) > 0);
        }
    }
}

