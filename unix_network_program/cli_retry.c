#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <error.h>
#include <sys/select.h>
#include <errno.h>

#include "utils.c"

static int signal_pairs[2];

void alarm_signal(int signal) {
    printf("receive alarm...\n");
    char empty = ' ';
    write(signal_pairs[1], &empty, 1);
    return;
};

int send_message(int fd, struct sockaddr_in *sa) {
    char words[100] = "Hello";
    if (sendto(fd, words, strlen(words), 0, (struct sockaddr*)sa, sizeof(*sa)) != strlen(words)) {
        perror("send error");
        return -1;
    }
    return 0;
};


int main() {
    int fd, recv_len, max_fd;
    fd_set read_set;
    char buffer[100];
    int timeout = 1;

    if (pipe(signal_pairs) < 0) {
        perror("pipe error");
    };

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket error");
        exit(1);
    };

    struct sockaddr_in sa;
    memset(&sa, 0, sizeof(struct sockaddr));
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = htonl(transfer_ip("127.0.0.1"));
    sa.sin_port = htons(8585);

    if (send_message(fd, &sa) < 0) 
        exit(1);

    signal(SIGALRM, alarm_signal);

    FD_ZERO(&read_set);

    max_fd = fd > signal_pairs[0] ? fd: signal_pairs[0];

    alarm(timeout);
    for(;;) {
        FD_SET(fd, &read_set);
        FD_SET(signal_pairs[0], &read_set);
        if (select(max_fd + 1, &read_set, NULL, NULL, NULL) < 0) {
            if (errno == EINTR) {
                continue;
            }
            perror("select error");
        };

        if (FD_ISSET(fd, &read_set)) {
            socklen_t len = 0;
            if (recv_len = recvfrom(fd, &buffer, sizeof(buffer), 0, (struct sockaddr*)&sa, &len) < 0) {
                perror("recv error");
            }
            printf("recv %s\n", buffer);
            break;
        }

        if (FD_ISSET(signal_pairs[0], &read_set)) {
            char n;
            read(signal_pairs[0], &n, 1);
            printf("receive timeout\n");
            send_message(fd, &sa);
            printf("send done\n");
            alarm(timeout*=2);
            printf("set timeout %d\n", timeout);
        }
    }
}

