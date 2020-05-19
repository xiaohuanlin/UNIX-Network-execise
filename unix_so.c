#include <sys/un.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>


int main() {
    int sockfd;
    socklen_t len;
    struct sockaddr_un addr, print_addr;
    char *path = "/tmp/test";

    sockfd = socket(AF_LOCAL, SOCK_STREAM, 0);
    unlink(path);

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_LOCAL;
    strncpy(addr.sun_path, path, sizeof(addr.sun_path)-1);
    bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));

    getsockname(sockfd, (struct sockaddr*)&print_addr, &len);
    printf("bind to %s\n", print_addr.sun_path);
}