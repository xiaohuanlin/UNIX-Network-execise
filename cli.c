#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <poll.h>
#include "utils.c"


int main() {
    int fd;
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        return -1;
    }

    struct sockaddr_in conn_addr;
    memset(&conn_addr, 0, sizeof(conn_addr));

    conn_addr.sin_family = AF_INET;
    conn_addr.sin_port = htons(8585);
    conn_addr.sin_addr.s_addr = htonl(transfer_ip("127.0.0.1"));

    get_socket_opt(fd);
    if (connect(fd, (struct sockaddr *)(&conn_addr), sizeof(conn_addr)) < 0) {
        printf("can\'t connect\n");
        exit(1);
    };
    get_socket_opt(fd);

    char buffer[100];
    int read_size;
    struct pollfd fds[2];

    fds[0].fd = fd;
    fds[0].events = POLLIN;
    fds[1].fd = 0; // stdin
    fds[1].events = POLLIN;

    while (1) {
        int nready, i;
        nready = poll(fds, 2, -1);
        for (i = 0; i < nready; i++) {
            if (fds[0].revents & POLLIN) {
                // data from server
                printf("server can read\n");
                read_size = read(fds[0].fd, buffer, 100);
                buffer[read_size] = 0;
                if (fputs(buffer, stdout) == EOF) {
                    exit(1);
                };
            }

            if (fds[1].revents & POLLIN) {
                // data from stdin
                printf("stdin can read\n");
                read_size = read(fds[1].fd, buffer, 100);
                buffer[read_size] = 0;
                fputs(buffer, stdout);
            }
        }
    }

    return 0;
}

