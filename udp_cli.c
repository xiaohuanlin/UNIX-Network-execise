#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include "utils.c"


int main() {
    int fd;
    struct sockaddr_in soaddr;
    memset(&soaddr, 0, sizeof(soaddr));

    soaddr.sin_family = AF_INET;
    soaddr.sin_addr.s_addr = htonl(transfer_ip("127.0.0.1"));
    soaddr.sin_port = htons(8585);

    fd = socket(AF_INET, SOCK_DGRAM, 0);

    char* words = "Hello";
    if (!sendto(fd, words, strlen(words), 0, (struct sockaddr*)&soaddr, sizeof(soaddr))) {
        printf("wrong\n");
    };
}