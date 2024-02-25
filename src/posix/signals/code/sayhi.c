#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

volatile sig_atomic_t counter = 0;

void sayhi(int signo) {
    if (++counter >= 3) {
        exit(1);
    }
    write(STDOUT_FILENO, "hi guys\n", 8);
}

int main() {
    signal(SIGINT, sayhi);

    char buf[1024];
    ssize_t n;
    while ((n = read(STDIN_FILENO, buf, sizeof(buf))) > 0) {
        write(STDOUT_FILENO, buf, n);
    }

    printf("read() returned a value <= 0\n");
}
