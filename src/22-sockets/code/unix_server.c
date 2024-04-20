#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int create_listener(const char* path) {
    struct sockaddr_un address = {.sun_family = AF_UNIX};
    if (strlen(path) >= sizeof(address.sun_path)) {
        fprintf(stderr, "pathname too long\n");
        exit(1);
    }
    strcpy(address.sun_path, path);

    int sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return sock;
    }

    // try to bind and listen
    if (bind(sock, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind");
        goto err;
    }
    if (listen(sock, SOMAXCONN) < 0) {
        perror("listen");
        goto err;
    }
    return sock;
err:
    close(sock);
    sock = -1;
    return sock;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s PATH\n", argv[0]);
        return 1;
    }
    int sock = create_listener(argv[1]);
    if (sock < 0) {
        return 1;
    }

    struct sockaddr_un address;
    socklen_t addrlen = sizeof(address);
    int connection = accept(sock, (struct sockaddr *) &address, &addrlen);
    printf("addrlen is %d\n", addrlen);
    printf("client address is %s\n", address.sun_path + 1);

    char* msg = "hello world\n";
    write(connection, msg, strlen(msg));

    close(connection);
    close(sock);
    unlink(argv[1]);
}
