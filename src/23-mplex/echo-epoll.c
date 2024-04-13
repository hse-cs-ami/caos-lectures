#define _GNU_SOURCE

#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "listener.h"


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

	struct epoll_event evt = {.events = EPOLLIN, .data.fd = sock};
	epoll_ctl(epollfd, EPOLL_CTL_ADD, sock, &evt);

	while (1) {
		int timeout = -1; // block indefinitely
		errno = 0;
		if (epoll_wait(epollfd, &evt, 1, timeout) < 1) {
			if (errno == EINTR) {
				continue;
			}
			perror("epoll_wait");
			return 1;
		}

		if (evt.data.fd == sock) {
            int connection = accept(sock, NULL, NULL);

            struct epoll_event evt = {.events = EPOLLIN,
                                      .data.fd = connection};
            epoll_ctl(epollfd, EPOLL_CTL_ADD, connection, &evt);
		} else {
			char buf[50] = {0};
            ssize_t res = read(evt.data.fd, buf, sizeof(buf) - 1);
            if (res == 0) {
                close(evt.data.fd);
            } else {
                write(evt.data.fd, buf, res);
                printf("fd %d: %s", evt.data.fd, buf);
            }
		}
	}
}

