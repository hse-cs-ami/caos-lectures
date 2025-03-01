#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>


int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s FILE\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_RDWR);
    off_t filesize = lseek(fd, 0, SEEK_END);
    printf("filesize: %llu\n", (unsigned long long)filesize);

    printf("pid: %d\n", getpid());
    char *ptr = mmap(0, filesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    printf("some file contents: %.12s\n", ptr);

    char buf[10];
    read(STDIN_FILENO, buf, sizeof(buf));

    puts("writing to the mapping");
    ptr[10] = buf[0];

    read(STDIN_FILENO, buf, sizeof(buf));
}
