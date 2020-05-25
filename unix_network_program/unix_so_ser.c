#include <sys/un.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <error.h>
#include "utils.c"


int main() {
    int sockfd, new_fd;
    int worker_fds[2];
    struct msghdr msg;
    socklen_t len;
    struct sockaddr_un addr, worker_addr;
    char *path = "/tmp/test";

    // init listener
    sockfd = socket(AF_LOCAL, SOCK_STREAM, 0);
    unlink(path);

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_LOCAL;
    strncpy(addr.sun_path, path, sizeof(addr.sun_path)-1);
    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        printf("bind error\n");
    };

    if (listen(sockfd, 100) < 0) {
        printf("listen error\n");
    };

    signal(SIGCHLD, logger_for_signal);
    if (socketpair(AF_LOCAL, SOCK_STREAM, 0, worker_fds) < 0) {
        perror("socketpair error");
    };

    int pid;
    if ((pid = fork()) < 0) {
        printf("call fork wrong\n");
    }

    if (pid == 0) {
        printf("start worker\n");
        // init workers
        close(sockfd);
        close(worker_fds[0]);

        union {
            struct cmsghdr cm;
            char control[CMSG_SPACE(sizeof(int))];
        } control_un;
        struct cmsghdr* cmptr;

        msg.msg_name = NULL;
        msg.msg_namelen = 0;
        msg.msg_control = control_un.control;
        msg.msg_controllen = sizeof(control_un.control);

        struct iovec io[1];
        char str_buf;
        io[0].iov_base = &str_buf;
        io[0].iov_len = 1;

        msg.msg_iov = io;
        msg.msg_iovlen = 1;

        if (recvmsg(worker_fds[1], &msg, 0) > 0) {
            if ((cmptr = CMSG_FIRSTHDR(&msg)) != NULL &&
                cmptr->cmsg_len == CMSG_LEN(sizeof(int))) {
                    if (cmptr->cmsg_level != SOL_SOCKET ||
                        cmptr->cmsg_type != SCM_RIGHTS) {
                            printf("error type");
                            // break;
                        }
                    int fd = *((int *)CMSG_DATA(cmptr));
                    char buffer[100];
                    if (read(fd, buffer, 100) < 0) {
                        printf("wrong in read\n");
                    }

                    printf("get data %s\n", buffer);
                };
        };
        exit(0);
    }

    close(worker_fds[1]);
    for (;;) {
        if ((new_fd = accept(sockfd, (struct sockaddr*)NULL, NULL))< 0) {
            printf("wrong accept\n");
            break;
        }
        printf("receive new connection\n");

        // make sure control info will align to cm
        union {
            struct cmsghdr cm;
            char control[CMSG_SPACE(sizeof(int))];
        } control_un;
        struct cmsghdr* cmptr;

        msg.msg_name = NULL;
        msg.msg_namelen = 0;
        msg.msg_control = control_un.control;
        msg.msg_controllen = sizeof(control_un.control);

        struct iovec io[1];
        char str_buf = {0};
        io[0].iov_base = &str_buf;
        io[0].iov_len = 1;
        msg.msg_iov = io;
        msg.msg_iovlen = 1;

        cmptr = CMSG_FIRSTHDR(&msg);
        cmptr->cmsg_len = CMSG_LEN(sizeof(int));
        cmptr->cmsg_level = SOL_SOCKET;
        cmptr->cmsg_type = SCM_RIGHTS;
        *((int *)(CMSG_DATA(cmptr))) = new_fd;

        // when we get new information, we should transfer to workers and listen information again
        if (sendmsg(worker_fds[0], &msg, 0) == -1) {
            perror("wrong in sendmsg");
        };
    }
}