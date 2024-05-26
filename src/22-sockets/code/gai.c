#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s NODE SERVICE\n", argv[0]);
        return 1;
    }

    // perform address resolution
    struct addrinfo* res = NULL;
    int gai_err;
    struct addrinfo hints = {
        .ai_family = PF_UNSPEC,
        .ai_socktype = SOCK_STREAM,
        .ai_flags = 0,   // try AI_ALL to include IPv6 on non-v6-enabled systems
    };
    if ((gai_err = getaddrinfo(argv[1], argv[2], &hints, &res))) {
        fprintf(stderr, "gai error: %s\n", gai_strerror(gai_err));
        return 2;
    }

    // iterate over the resulting addresses
    for (struct addrinfo* ai = res; ai; ai = ai->ai_next) {
        struct protoent* proto = getprotobynumber(ai->ai_protocol);
        if (proto) {
            printf("ai_flags=%d, ai_family=%d, ai_socktype=%d, ai_protocol=%s\n",
                   ai->ai_flags,
                   ai->ai_family,
                   ai->ai_socktype,
                   proto->p_name);
        }

        char host[1024], port[10];
        if ((gai_err = getnameinfo(ai->ai_addr,
                                   ai->ai_addrlen,
                                   host,
                                   sizeof(host),
                                   port,
                                   sizeof(port),
                                   NI_NUMERICHOST | NI_NUMERICSERV))) {
            fprintf(stderr, "getnameinfo error: %s\n", gai_strerror(gai_err));
            return 3;
        }

        printf("\taddress: %s, port: %s\n", host, port);
    }
    freeaddrinfo(res);
}
