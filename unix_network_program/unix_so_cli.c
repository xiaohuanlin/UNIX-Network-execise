#include <sys/un.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <error.h>


int main() {
    int sockfd;
    socklen_t len;
    struct sockaddr_un addr, print_addr;
    char *path = "/tmp/test";

    sockfd = socket(AF_LOCAL, SOCK_STREAM, 0);

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_LOCAL;
    strncpy(addr.sun_path, path, sizeof(addr.sun_path)-1);
    if (connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect error");
        exit(1);
    };
    printf("connect success\n");

    char *test_word = "hello";
    if (write(sockfd, test_word, strlen(test_word)) < 0) {
        printf("write wrong");
    };
}