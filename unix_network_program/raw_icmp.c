#include <netinet/ip_icmp.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <sys/epoll.h>
#include <error.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>


int main() {
    int fd, efd, nfd, file;
    struct epoll_event events[10], icmp_event;
    if ((fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
        perror("socket error");
    };
    setuid(getuid());

    efd = epoll_create1(0);
    icmp_event.data.fd = fd;
    icmp_event.events = EPOLLIN;
    if (epoll_ctl(efd, EPOLL_CTL_ADD, fd, &icmp_event) < 0) {
        perror("epoll ctl error");
    };

    char buffer[200];
    file = open("test", O_RDWR | O_CREAT);

    while (1) {
        if ((nfd = epoll_wait(efd, events, 10, -1)) < 0) {
            perror("epoll error");
        };
        int i, len;
        for (i = 0; i < nfd; i++) {
            if (events[i].data.fd == fd) {
                if ((len = read(fd, buffer, 200)) < 0) {
                    perror("read error");
                    exit(1);
                };

                if (write(file, buffer, len) < 0) {
                    perror("write error");
                };
                close(file);
                exit(0);
            }
        }
    }
}