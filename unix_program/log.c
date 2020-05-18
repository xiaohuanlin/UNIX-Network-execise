#include "apue.h"
#include <errno.h>
#include <stdarg.h>
#include <syslog.h>

static void log_doit(int, int, int, const char *, va_list);

// Caller must define and set this: nonzero if interactive, zero if daemon
extern int log_to_stderr;

// Initialize syslog(), if running as daemon
void log_open(const char *ident, int option, int facility)
{
    if (log_to_stderr == 0)
        openlog(ident, option, facility);
}

// Print a message with system errno and return/ Nonfatal error related to a system call
void log_ret(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    log_doit(1, errno, LOG_ERR, fmt, ap);
    va_end(ap);
}

// Print a message and terminate/ Fatal error related to a system call
void log_sys(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    log_doit(1, errno, LOG_ERR, fmt, ap);
    va_end(ap);
    exit(2);
}

// Print a message and return/ Nonfatal error unrelated to a system call
void log_msg(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    log_doit(0, 0, LOG_ERR, fmt, ap);
    va_end(ap);
}

// Print a message and terminate/ Fatal error unrelated to a system call
void log_quit(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    log_doit(0, 0, LOG_ERR, fmt, ap);
    va_end(ap);
    exit(2);
}

// Print a message and terminate/ Fatal error unrelated to a system call/ with error
void log_exit(int error, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    log_doit(1, error, LOG_ERR, fmt, ap);
    va_end(ap);
    exit(1);
}

// Print a message and return to caller/ Caller specifies "errnoflag"
static void
log_doit(int errnoflag, int error, int priority, const char *fmt, va_list ap)
{
    char buf[MAXLINE];

    vsnprintf(buf, MAXLINE - 1, fmt, ap);
    if (errnoflag)
    {
        snprintf(buf + strlen(buf), MAXLINE - strlen(buf) - 1, ": %s", strerror(error));
    }
    if (log_to_stderr) {
        fflush(stdout);
        fputs(buf, stderr);
        fflush(stderr);
    } else {
        syslog(priority, "%s", buf);
    }
}