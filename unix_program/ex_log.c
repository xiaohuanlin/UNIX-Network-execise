#include <syslog.h>

int main() {
    static char* pname = "user_test";
    openlog(pname, LOG_NDELAY | LOG_CONS | LOG_PID, LOG_LOCAL2);
    syslog(LOG_ERR | LOG_LOCAL0, "this is test service %d", 1);
    syslog(LOG_INFO, "this is test service %d", 1);
    closelog();
}