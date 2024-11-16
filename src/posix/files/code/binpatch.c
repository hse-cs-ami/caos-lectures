#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char usage[] =
    "Usage: binpatch FILE OFFSET DATA\n"
    "With empty DATA, truncates FILE at OFFSET.\n";

int main(int argc, char *argv[]) {
    if (argc != 4) {
        write(STDERR_FILENO, usage, strlen(usage));
        return EXIT_FAILURE;
    }

    const char *name = argv[1];
    int offset = atoi(argv[2]);
    const char *data = argv[3];

    int fd = open(name, O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR); // если передать название несуществующего файла, то благодаря O_CREAT он будет создан
    if (fd < 0) {
        perror(name);
        return EXIT_FAILURE;
    }

    if (lseek(fd, offset, SEEK_SET) < 0) {
        perror("seek");
        return EXIT_FAILURE;
    }
    
    if (write(fd, data, strlen(data)) < 0) {
        perror("write");
        return EXIT_FAILURE;
    }

    if (!strlen(data)) {
        off_t position = lseek(fd, 0, SEEK_CUR);  // lseek возвращает смещение от начала файла
        ftruncate(fd, position);  // обрезает файл
    }

    // можно не закрывать файл, поскольку мы выходим из программы, и ОС подчистит все наши файловые дескрипторы 
}
