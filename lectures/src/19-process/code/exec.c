#include <stdio.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

int main() {
    puts("executing echo");
    if (fork() == 0) {
        // child process
        int fd = open("output", O_CREAT | O_WRONLY | O_TRUNC, 0666);
        if (fd < 0) {
            perror("output");
            return 127;
        }
        if (dup2(fd, STDOUT_FILENO) < 0) {
            perror("dup2");
            return 127;
        }
        close(fd);

        char* cmd = "/bin/echo";
        char* argv[] = {"echo", "42", NULL};
        char* envp[] = {NULL};
        execve(cmd, argv, envp);
        perror(cmd);
        return 127;
    }
    // parent process
    int status;
    wait(&status);
    printf("child exited with status: %d\n", WEXITSTATUS(status));
}
