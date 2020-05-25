#include <netinet/in.h>
#include <sys/socket.h>
#include <stdio.h>


int main() {
    int tcp_fd, udp_fd;
    tcp_fd = socket(AF_INET, SOCK_STREAM, 0);
    udp_fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (tcp_fd < 0 || udp_fd < 0) {
        return -1;
    }

    int int_value;
    int len = sizeof(int_value);
    printf("tcp rcv buffer res=%d ", getsockopt(tcp_fd, SOL_SOCKET, SO_RCVBUF, &int_value, &len));
    printf("data=%d\n", int_value);
    printf("tcp snd buffer res=%d ", getsockopt(tcp_fd, SOL_SOCKET, SO_SNDBUF, &int_value, &len));
    printf("data=%d\n", int_value);
    printf("udp rcv buffer res=%d ", getsockopt(udp_fd, SOL_SOCKET, SO_RCVBUF, &int_value, &len));
    printf("data=%d\n", int_value);
    printf("udp snd buffer res=%d ", getsockopt(udp_fd, SOL_SOCKET, SO_SNDBUF, &int_value, &len));
    printf("data=%d\n", int_value);

}