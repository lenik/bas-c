#include "env.h"

#include "../io/path.h"

#include <sys/stat.h>

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static char *pathv;

/* the result should be free-ed if it's non-null. */
char *path_find(const char *name) {
    if (! pathv) {
        char *pathenv = getenv("PATH");
        if (pathenv == NULL)
            pathenv = "";
        int pathenvlen = strlen(pathenv);

        pathv = malloc(pathenvlen + 2);
        strcpy(pathv, pathenv);
        pathv[pathenvlen + 1] = '\0';

        char *p = pathv;
        while (*p) {
            if (*p == ':')
                *p = '\0';
            p++;
        }
    }

    char *p = pathv;
    char join[PATH_MAX];
    struct stat sb;

    while (*p) {
        strcpy(join, p);
        strcat(join, "/");
        strcat(join, name);

        if (stat(join, &sb) >= 0) {     /* file exists. */
            mode_t mode = sb.st_mode;
            int xbits = mode & (S_IXUSR | S_IXGRP | S_IXOTH);
            if (xbits != 0)
                return strdup(join);
        }

        p += strlen(p) + 1;
    }

    return NULL;
}

char *path_find_norm(const char *name) {
    assert(name != NULL);

    char *norm;

    if (*name != '/') {
        char *path_expansion = path_find(name);
        if (path_expansion != NULL) {
            norm = path_normalize(path_expansion);
            free(path_expansion);
            return norm;
        }
    }

    norm = path_normalize(name);
    return norm;
}

char *etempl(char *path_out, size_t path_out_size, const char *templ, const char *tmp_dir) {
    // 1. Determine the base temp directory (e.g., /tmp, /var/tmp, or user defined)
    if (templ == NULL)
        templ = "XXXXXX";
    
    if (templ[0] == '/') {
        strncpy(path_out, templ, path_out_size);
    } else {
        if (tmp_dir == NULL) {
            tmp_dir = getenv("TMP");
            if (tmp_dir == NULL) {
                tmp_dir = getenv("TEMP");
                if (tmp_dir == NULL) {
                    tmp_dir = "/tmp";
                }
            }
        }
        snprintf(path_out, path_out_size, "%s/%s", tmp_dir, templ);
    }

    return path_out;
}

int emkstemp(char *path_out, size_t path_out_size, const char *templ, const char *tmp_dir) {
    etempl(path_out, path_out_size, templ, tmp_dir);
    int fd = mkstemp(path_out);
    return fd;
}


char *emkdtemp(char *path_out, size_t path_out_size, const char *templ, const char *tmp_dir) {
    etempl(path_out, path_out_size, templ, tmp_dir);
    char *tmp = mkdtemp(path_out);
    return tmp;
}

FILE *emktemp_open(char *path_out, size_t path_out_size, const char *templ, const char *tmp_dir, const char *mode) {
    etempl(path_out, path_out_size, templ, tmp_dir);
    int fd = mkstemp(path_out);
    if (fd == -1) {
        perror("mktemp_open: Failed to create temp file");
        return NULL;
    }
    if (mode == NULL)
        mode = "w";
    FILE *f = fdopen(fd, mode);
    return f;
}
