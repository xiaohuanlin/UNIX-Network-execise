#include <stdlib.h>
#include <sys/wait.h>
#include <stdio.h>


int main() {
    // If the change of value on parent process will influence the value on child?
    int test = 1;
    int pid;
    if ((pid = fork()) < 0) {
        printf("wrong in fork\n");
        exit(1);
    }
    printf("this is %d\n", pid);
    if (pid == 0) {
        int i = 0;
        while ( i < 1e3) 
            i++;
        printf("child %d\n", test);
        exit(0);
    }

    test = 2;
    printf("wait\n");
    waitpid(pid, NULL, 0);
}