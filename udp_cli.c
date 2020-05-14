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
    int send_buffer_len = 64e3;
    if (setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &send_buffer_len, sizeof(send_buffer_len)) < 0) {
        printf("set opt wrong\n");
    };

    // test how many words could be sent
    // if word_size exceed 2^16, then quit to send
    int word_size = 70000;
    char words[word_size];
    int i;
    for (i = 0; i < word_size - 1; i++ ) {
        words[i] = '1';
    }
    words[word_size] = '\0';
    if (!sendto(fd, words, strlen(words), 0, (struct sockaddr*)&soaddr, sizeof(soaddr))) {
        printf("wrong\n");
    };
}