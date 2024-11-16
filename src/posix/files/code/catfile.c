#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

void catfile(int sourcefd) {
    char buf[10000];
    ssize_t result;
    while ((result = read(sourcefd, buf, sizeof(buf))) > 0) {
        write(STDOUT_FILENO, buf, result);
    }
}

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        catfile(STDIN_FILENO);
    } else {
        for (int f = 1; f < argc; ++f) {
            int fd = open(argv[f], O_RDONLY); // в argv[f] путь к файлу
            if (fd < 0) {
                perror(argv[f]);
                return EXIT_FAILURE;  // EXIT_FAILURE = 1
            }
            catfile(fd);
            close(fd);
        }
    }
}
