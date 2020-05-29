#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <error.h>
#include <fcntl.h>
#include <string.h>


static int count;
static int get_sig;


void sig_io(int sig) {
    printf("count=%d until we get sigio\n", count);
    get_sig = 1;
}

int main() {
    fcntl(STDOUT_FILENO, F_SETOWN, getpid());

    int flags = fcntl(STDOUT_FILENO, F_GETFL);
    fcntl(STDOUT_FILENO, F_SETFL, flags | O_ASYNC | O_NONBLOCK);

    signal(SIGIO, sig_io);

    char words[] = "Hello\n";
    write(STDOUT_FILENO, words, strlen(words));
    for (;get_sig == 0;count++);
}