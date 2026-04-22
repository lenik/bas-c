#include "i18n.h"
#include "bas/proc/env.h"
#include <limits.h>
#include <locale.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

void va_init_i18n(const char* locale_dir, ...) {
    char* exe_path = self_exe_dup();
    char po_dir[PATH_MAX];
    const char* domain_dir = locale_dir;

    setlocale(LC_ALL, "");

    char* slash = strrchr(exe_path, '/');
    if (slash) {
        *slash = '\0';
        int n = snprintf(po_dir, sizeof(po_dir), //
                         "%s/po", exe_path);
        struct stat st;
        if (stat(po_dir, &st) == 0 //
            && S_ISDIR(st.st_mode)) {
            domain_dir = po_dir;
        }
    }

    va_list args;
    va_start(args, locale_dir);
    const char* arg;
    while (arg = va_arg(args, const char*)) {
        bindtextdomain(arg, domain_dir);
    }
    va_end(args);

    free(exe_path);
}
