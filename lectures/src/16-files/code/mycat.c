#include <unistd.h>

int main() {
    char c; // наш буфер в один байт
    while (read(STDIN_FILENO, &c, sizeof(c)) > 0) { // STDIN_FILENO = 0
        write(STDOUT_FILENO, &c, sizeof(c));        // STDOUT_FILENO = 1
    }
}
