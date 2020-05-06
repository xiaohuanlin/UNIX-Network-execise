#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
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

    if (connect(fd, (struct sockaddr *)(&conn_addr), sizeof(conn_addr)) < 0) {
        printf("can\'t connect\n");
        exit(1);
    };

    char buffer[100];
    int read_size;
    while ((read_size = read(fd, buffer, 100)) > 0) {
        buffer[read_size] = 0;
        if (fputs(buffer, stdout) == EOF) {
            exit(1);
        };
    }
    return 0;
}

