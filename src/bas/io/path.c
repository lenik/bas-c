#include "path.h"

#include <sys/stat.h>

#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../base/str.h"

char *path_normalize(const char *path) {
    char real[PATH_MAX];
    if (realpath(path, real) == NULL)
        return NULL;
    else
        return str_dup(real);
}
