#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int picoshell(char **cmds[]) {
    int num = 0;
    while (cmds[num] != NULL)
        num++;
    
    int fd[2];
    int prev_fd = -1;
    pid_t pids[num];

    for (int i = 0; i < num; i++) {
        if (i < num - 1) {
            if (pipe(fd) == -1) {
                perror("pipe");
                return 1;
            }
        }

        pids[i] = fork();
        if (pids[i] == -1) {
            perror("fork");
            return 1;
        }

        if (pids[i] == 0) { // Child process
            if (prev_fd != -1) {
                dup2(prev_fd, STDIN_FILENO);
                close(prev_fd);
            }
            if (i < num - 1) {
                dup2(fd[1], STDOUT_FILENO);
                close(fd[1]);
                close(fd[0]);
            }
            execvp(cmds[i][0], cmds[i]);
            perror("execvp");
            exit(1);
        }

        if (prev_fd != -1) {
            close(prev_fd);
        }
        if (i < num - 1) {
            close(fd[1]);
            prev_fd = fd[0];
        }
    }

    for (int i = 0; i < num; i++) {
        wait(NULL);
    }

    return 0;
}
