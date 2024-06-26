#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int create_listener(char* service) {
    struct addrinfo* res = NULL;
    int gai_err;
    struct addrinfo hint = {
        .ai_family = AF_INET6,
        .ai_socktype = SOCK_STREAM,
        .ai_flags = AI_PASSIVE,
    };
    if ((gai_err = getaddrinfo(NULL, service, &hint, &res))) {
        fprintf(stderr, "gai error: %s\n", gai_strerror(gai_err));
        return -1;
    }
    int sock = -1;
    for (struct addrinfo* ai = res; ai; ai = ai->ai_next) {
        sock = socket(ai->ai_family, ai->ai_socktype, 0);
        if (sock < 0) {
            perror("socket");
            continue;
        }
        int one = 1;
        if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one))) {
            perror("setsockopt");
            goto err;
        }
        if (bind(sock, ai->ai_addr, ai->ai_addrlen) < 0) {
            perror("bind");
            goto err;
        }
        if (listen(sock, 1) < 0) {
            perror("listen");
            goto err;
        }
        break;
err:
        close(sock);
        sock = -1;
    }
    freeaddrinfo(res);
    return sock;
}

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
            close(sock);   // we won't be accepting anything here
            char* msg = "hello world";
            write(connection, msg, strlen(msg));
            return 0;
        } else {
            close(connection);
            while (waitpid(-1, NULL, WNOHANG) > 0)
                ;
        }
    }
    close(sock);
}
