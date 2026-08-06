#include "syslog.h"

#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

int syslog_facility = LOG_USER;

void _syslog_x(const char *ident, int option, int level,
               const char *format, ...) {
    char msgbuf[1024];
    va_list ap;

    va_start(ap, format);
    vsnprintf(msgbuf, sizeof(msgbuf), format, ap);
    va_end(ap);

    openlog(ident, option, syslog_facility);
    syslog(level, "%s", msgbuf);
    closelog();
}

void _syslog_x_perror(const char *ident, int option, int level,
                      const char *format, ...) {
    char msgbuf[1024];
    char *errmsg;
    int errlen;

    errmsg = strerror(errno);
    errlen = strlen(errmsg);

    va_list ap;
    va_start(ap, format);
    vsnprintf(msgbuf, sizeof(msgbuf) - (errlen + 2), format, ap);
    va_end(ap);

    strcat(msgbuf, ": ");
    strcat(msgbuf, errmsg);

    openlog(ident, option, syslog_facility);
    syslog(level, "%s", msgbuf);
    closelog();
}
