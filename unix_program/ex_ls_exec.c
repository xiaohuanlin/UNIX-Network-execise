#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>


int main() {
    int pid;
    if ((pid = fork()) < 0) {
        printf("fork error\n");
    }

    if (pid == 0) {
        // close all fd and redirect stdin stdout stderr
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);
        // open("/dev/null", O_RDONLY);
        // open("/dev/null", O_RDWR);
        // open("/dev/null", O_RDWR);
    }

    int i = 0;
    while (i < 1e8) {
        i++;
    };
    // test if we can see the fd infor
    if (execle("/usr/bin/ls", "ls", "/data/", NULL) < 0){
        printf("wrong exec\n");
    };
}