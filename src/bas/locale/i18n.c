#include "i18n.h"
#include <limits.h>
#include <locale.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

void va_init_i18n(const char* exe_path, const char* locale_dir, const char* text_domains, ...) {
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
    va_start(args, text_domains);
    const char* arg = text_domains;
    while (arg) {
        bindtextdomain(arg, domain_dir);
        arg = va_arg(args, const char*);
    }
    va_end(args);

    textdomain(text_domains);
}
