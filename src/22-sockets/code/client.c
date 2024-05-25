#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int create_connection(char* node, char* service) {
    struct addrinfo* res = NULL;
    int gai_err;
    struct addrinfo hint = {
        .ai_family = AF_UNSPEC,       // можно и AF_INET, и AF_INET6
        .ai_socktype = SOCK_STREAM,   // но мы хотим поток (соединение)
    };
    if ((gai_err = getaddrinfo(node, service, &hint, &res))) {
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
        if (connect(sock, ai->ai_addr, ai->ai_addrlen) < 0) {
            perror("connect");
            close(sock);
            sock = -1;
            continue;
        }
        break;
    }
    freeaddrinfo(res);
    return sock;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s NODE SERVICE\n", argv[0]);
        return 1;
    }
    int sock = create_connection(argv[1], argv[2]);
    if (sock < 0) {
        return 1;
    }
    char buf[1024] = {0};
    if (read(sock, &buf, sizeof(buf) - 1) > 0) {
        printf("received message: %s\n", buf);
    }
    close(sock);
}
