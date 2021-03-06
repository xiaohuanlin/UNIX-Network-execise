#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/wait.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <stdarg.h>
#include <sys/uio.h>


// Convert the string ip address to uint32 number. Negtive number means error
uint32_t transfer_ip(const char *ip_address) {
    uint8_t ip_value[4] = {};
    int current_ip_slice = 0;
    char current_ip_cahr;

    int i;
    for (i = 0; i < strlen(ip_address); i++) {
        if ((current_ip_cahr = ip_address[i]) == '.') {
            current_ip_slice += 1;
            if (*ip_value > 255) {
                // overflow
                return -1;
            }
            continue;
        }
        ip_value[current_ip_slice] = ip_value[current_ip_slice]*10 + (current_ip_cahr - '0');
    }

    if (current_ip_slice != 3) {
        return -1;
    }

    uint32_t true_value = ip_value[0] << 24 | ip_value[1] << 16 | ip_value[2] << 8 | ip_value[3];
    return true_value;
}


void logger_for_signal(int signal) {
    pid_t pid;
    // by call wait/waitpid, we could realese the Zombie Progress
    // waitpid could catch all SIGCLD at same time
    while ((pid = waitpid(-1, NULL, WNOHANG)) > 0) {
        printf("receive sig %d from pid %d\n", signal, pid);
    };
    return;
};


int get_socket_opt(int fd) {
    int value, size;
    size = sizeof(value);
    if (getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &value, &size)) {
        return -1;
    };
    printf("snd buffer value %d\n", value);

    if (getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &value, &size)) {
        return -1;
    };
    printf("rcv buffer value %d\n", value);

    if (getsockopt(fd, IPPROTO_TCP, TCP_MAXSEG, &value, &size)) {
        return -1;
    };
    printf("MSS value %d\n", value);
}


void connect_alarm(int sig) {
    printf("receive alarm\n");
    return;
}


// only support for %s
void echo_for_str_list(int fd, const char *fmt, ...) {
    int count = 0;
    const char* ori;

    va_list ap;
    ori = fmt;

    // get the number of list
    va_start(ap, fmt);
    for (; *fmt; fmt++) {
        if (*fmt == '%') {
            switch (*(++fmt))
            {
            case 's':
                count++;
                break;
            default:
                break;
            }
        } else {
            count ++;
        }
    }
    va_end(ap);

    struct iovec str_vec[count];
    count = 0;
    fmt = ori;

    va_start(ap, fmt);
    for (; *fmt; fmt++) {
        if (*fmt == '%') {
            switch (*(++fmt))
            {
            case 's': {
                char *tmp = va_arg(ap, char*);
                struct iovec io_tmp = {
                    tmp,
                    strlen(tmp)
                };
                str_vec[count++] = io_tmp;
                break;
            }
            default:
                break;
            }
        } else {
            struct iovec io_tmp = {
                fmt,
                1
            };
            str_vec[count++] = io_tmp;
        }
    }
    va_end(ap);

    writev(fd, str_vec, sizeof(str_vec)/sizeof(struct iovec));
}