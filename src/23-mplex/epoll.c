#define _GNU_SOURCE

#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int create_listener(char* service) {
    struct addrinfo *res = NULL;
    int gai_err;
    struct addrinfo hint = {
        .ai_family = AF_UNSPEC,
        .ai_socktype = SOCK_STREAM,
        .ai_flags = AI_PASSIVE,
    };
    if ((gai_err = getaddrinfo(NULL, service, &hint, &res))) {
        fprintf(stderr, "gai error: %s\n", gai_strerror(gai_err));
        return -1;
    }
    int sock = -1;
    for (struct addrinfo *ai = res; ai; ai = ai->ai_next) {
        sock = socket(ai->ai_family, ai->ai_socktype | SOCK_NONBLOCK, 0);
        if (sock < 0) {
            perror("socket");
            continue;
        }
        int one = 1;
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
        if (bind(sock, ai->ai_addr, ai->ai_addrlen) < 0) {
            perror("bind");
            close(sock);
            sock = -1;
            continue;
        }
        if (listen(sock, SOMAXCONN) < 0) {
            perror("listen");
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
    if (argc != 2) {
        fprintf(stderr, "Usage: %s SERVICE\n", argv[0]);
        return 1;
    }
    int sock = create_listener(argv[1]);
    if (sock < 0) {
        return 1;
    }

    int epollfd = epoll_create1(0);
    if (epollfd < 0) {
        perror("epoll");
        return 1;
    }

	struct epoll_event evt = {.events = EPOLLIN | EPOLLET, .data.fd = sock};
	epoll_ctl(epollfd, EPOLL_CTL_ADD, sock, &evt);

	while (1) {
		int timeout = -1; // block indefinitely
		errno = 0;
		if (epoll_wait(epollfd, &evt, 1, timeout) < 1) {
			if (errno == EINTR) {
				continue;
			}
			perror("select");
			return 1;
		}

		if (evt.data.fd == sock) {
            while (1) {
                int connection = accept4(sock, NULL, NULL, SOCK_NONBLOCK);
                if (connection == -1) {
                    break;
                }

                struct epoll_event evt = {.events = EPOLLIN | EPOLLET,
                                          .data.fd = connection};
                epoll_ctl(epollfd, EPOLL_CTL_ADD, connection, &evt);
            }
		} else {
			char buf[2] = {0};
            ssize_t res;
            while ((res = read(evt.data.fd, buf, sizeof(buf) - 1)) >= 0) {
                if (res == 0) {
                    close(evt.data.fd);
                } else {
                    write(evt.data.fd, buf, res);
                    printf("fd %d: %s", evt.data.fd, buf);
                }
            }
		}
	}
}

