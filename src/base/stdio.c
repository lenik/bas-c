#include "stdio.h"

#include <stdarg.h>
#include <stdio.h>

bool errorf(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    return false;
}

