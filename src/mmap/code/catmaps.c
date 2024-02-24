#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main() {
    int fd = open("/proc/self/maps", O_RDONLY);
    if (fd < 0) {
        perror("failed to open /proc/self/maps");
        return 1;
    }
    char buf[1024];
    ssize_t size;
    while ((size = read(fd, buf, sizeof(buf))) > 0) {
        write(STDOUT_FILENO, buf, size);
    }
}
