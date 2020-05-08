#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/select.h>

#include "utils.c"


void do_task_with_fork(int listen_fd, char* words) {
    while (1) {
        int new_fd = accept(listen_fd, (struct sockaddr *)NULL, NULL);
        if (errno == EINTR) {
            continue;
        }

        pid_t pid;
        if ((pid = fork()) == 0) {
            // reduce the reference of listenfd
            close(listen_fd);
            printf("get new fd: %d\n", listen_fd);
            write(new_fd, words, strlen(words) + 1);
            close(new_fd);
            printf("close new fd\n");
            exit(0);
        } else {
            // reduce the reference of newfd
            close(new_fd);
        }
    }
};


void do_task_with_select(int listen_fd, char* words) {
    int fd_init = 0;
    fd_set rset;
    int max_fd;
    int capacity = 2;
    int cur_index = 0;
    int* fds = malloc(capacity * sizeof(int));

    while (1) {
        if (fd_init == 0) {
            FD_ZERO(&rset);
            FD_SET(listen_fd, &rset);
            max_fd = listen_fd;
            fd_init = 1;
        }

        fd_set result_set = rset;
        if (select(max_fd + 1, &result_set, NULL, NULL, NULL) < 0) {
            printf("select error\n");
            exit(1);
        };

        if (FD_ISSET(listen_fd, &result_set)) {
            printf("listen can read\n");
            int new_fd = accept(listen_fd, (struct sockaddr *)NULL, NULL);
            if (errno == EINTR) {
                continue;
            }
            FD_SET(new_fd, &rset);
            fds[cur_index++] = new_fd;
            max_fd = max_fd > new_fd ? max_fd: new_fd; 

            // if capacity is not enough, then double it
            if (cur_index >= capacity) {
                capacity *= 2;
                int* temp = malloc(capacity * sizeof(int));
                memcpy(temp, fds, sizeof(fds));
                free(fds);
                fds = temp;
            }

            printf("get new fd: %d\n", new_fd);
            write(new_fd, words, strlen(words) + 1);
        }

        int i;
        char buffer[20];
        for (i = 0; i < cur_index; i++) {
            int new_fd = fds[i];
            if (FD_ISSET(new_fd, &result_set)) {
                printf("fd can read\n");
                read(new_fd, buffer, 20);
                write(new_fd, buffer, strlen(buffer) + 1);
                close(new_fd);
                fds[i] = 0;
                FD_CLR(new_fd, &rset);
            }
        }
    }
};


int main() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8585);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(fd, (struct sockaddr*)(&servaddr), sizeof(servaddr)) < 0) {
        printf("can\'t bind\n");
        exit(1);
    };

    int max_listen_queue = 1;
    if (listen(fd, max_listen_queue) < 0) {
        printf("can\'t listen\n");
        exit(1);
    }

    printf("listen in %d\n", ntohs(servaddr.sin_port));
    int new_fd;
    int pid;
    char buffer[] = "Hello\n";
    signal(SIGCLD, logger_for_signal);
    
    // do_task_with_fork(fd, buffer);
    do_task_with_select(fd, buffer);
}

