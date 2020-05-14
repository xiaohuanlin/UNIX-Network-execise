#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include "utils.c"


int main() {
    int fd;
    struct sockaddr_in soaddr;
    memset(&soaddr, 0, sizeof(soaddr));

    soaddr.sin_family = AF_INET;
    soaddr.sin_addr.s_addr = htonl(transfer_ip("127.0.0.1"));
    soaddr.sin_port = htons(8585);

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (bind(fd, (struct sockaddr*)&soaddr, sizeof(soaddr)) < 0) {
        printf("error in bind\n");
        exit(1);
    };

    socklen_t size = sizeof(soaddr);
    char words[100] = {};
    if (recvfrom(fd, words, sizeof(words), 0, (struct sockaddr*)&soaddr, &size) < 0) {
        printf("wrong in recv\n");
        exit(1);
    };

    printf("%s\n", words);
}