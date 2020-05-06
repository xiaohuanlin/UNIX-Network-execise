#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "utils.c"


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
    char buffer[] = "Hello\n";
    while (1) {
        new_fd = accept(fd, (struct sockaddr *)NULL, NULL);
        printf("get new fd: %d\n", fd);
        write(new_fd, buffer, strlen(buffer) + 1);
        close(new_fd);
        printf("close new fd\n");
    };
}

