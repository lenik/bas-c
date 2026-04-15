#include "env.h"

#include "../io/path.h"

#include <sys/stat.h>

#include <assert.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#if defined(_WIN32)
#include <windows.h>
#elif defined(__linux__)
#include <unistd.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#include <string.h>
#endif

static char* pathv;

/* the result should be free-ed if it's non-null. */
char* path_find(const char* name) {
    if (!pathv) {
        char* pathenv = getenv("PATH");
        if (pathenv == NULL)
            pathenv = "";
        int pathenvlen = strlen(pathenv);

        pathv = malloc(pathenvlen + 2);
        strcpy(pathv, pathenv);
        pathv[pathenvlen + 1] = '\0';

        char* p = pathv;
        while (*p) {
            if (*p == ':')
                *p = '\0';
            p++;
        }
    }

    char* p = pathv;
    char join[PATH_MAX];
    struct stat sb;

    while (*p) {
        strcpy(join, p);
        strcat(join, "/");
        strcat(join, name);

        if (stat(join, &sb) >= 0) { /* file exists. */
            mode_t mode = sb.st_mode;
            int xbits = mode & (S_IXUSR | S_IXGRP | S_IXOTH);
            if (xbits != 0)
                return strdup(join);
        }

        p += strlen(p) + 1;
    }

    return NULL;
}

char* path_find_norm(const char* name) {
    assert(name != NULL);

    char* norm;

    if (*name != '/') {
        char* path_expansion = path_find(name);
        if (path_expansion != NULL) {
            norm = path_normalize(path_expansion);
            free(path_expansion);
            return norm;
        }
    }

    norm = path_normalize(name);
    return norm;
}

char* etempl(char* path_out, size_t path_out_size, const char* templ, const char* tmp_dir) {
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

int emkstemp(char* path_out, size_t path_out_size, const char* templ, const char* tmp_dir) {
    etempl(path_out, path_out_size, templ, tmp_dir);
    int fd = mkstemp(path_out);
    return fd;
}

char* emkdtemp(char* path_out, size_t path_out_size, const char* templ, const char* tmp_dir) {
    etempl(path_out, path_out_size, templ, tmp_dir);
    char* tmp = mkdtemp(path_out);
    return tmp;
}

FILE* emktemp_open(char* path_out, size_t path_out_size, const char* templ, const char* tmp_dir,
                   const char* mode) {
    etempl(path_out, path_out_size, templ, tmp_dir);
    int fd = mkstemp(path_out);
    if (fd == -1) {
        perror("mktemp_open: Failed to create temp file");
        return NULL;
    }
    if (mode == NULL)
        mode = "w";
    FILE* f = fdopen(fd, mode);
    return f;
}

#include <stddef.h>

#if defined(_WIN32)
#include <windows.h>
#elif defined(__linux__)
#include <unistd.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#include <string.h>
#endif

/**
 * Find the path of the current executable.
 *
 * Returns:
 * - Success: Number of bytes written to buffer (excluding null terminator).
 * - Truncation: Required buffer size (excluding null terminator) to hold the full path.
 * - Failure: 0.
 *
 * Behavior: Similar to snprintf, if return value >= size, the output was truncated.
 */
size_t get_self_exe_path(char* buffer, size_t size) {
    if (buffer == NULL || size == 0) {
        // Return a suggested size if no buffer is provided
        return 1024;
    }

#if defined(_WIN32)
    // GetModuleFileNameA returns characters written.
    // If buffer is too small, it returns 'size' and sets last error to ERROR_INSUFFICIENT_BUFFER.
    DWORD ret = GetModuleFileNameA(NULL, buffer, (DWORD)size);
    if (ret == 0)
        return 0;
    // If ret == size, Windows truncated the string and didn't null-terminate.
    if (ret == (DWORD)size) {
        /* Return size * 2 to signal truncation and suggest a larger buffer.
           We use a small cap or check for overflow if you want to be pedantic. */
        return (size_t)size * 2;
    }
    return (size_t)ret;

#elif defined(__linux__)
    // readlink does not null-terminate.
    // We pass size - 1 to leave room for the manual null terminator.
    ssize_t len = readlink("/proc/self/exe", buffer, size - 1);
    if (len < 0)
        return 0;

    buffer[len] = '\0';

    // If len equals the requested capacity (size - 1), it might be truncated.
    return (size_t)len;

#elif defined(__APPLE__)
    uint32_t bufsize = (uint32_t)size;
    if (_NSGetExecutablePath(buffer, &bufsize) == 0) {
        return strlen(buffer);
    }
    // If buffer is too small, _NSGetExecutablePath returns -1
    // and updates bufsize to the required size.
    return (size_t)bufsize;
#else
    return 0;
#endif
}

/**
 * self_exe: Get the path of the current executable.
 * Returns a heap-allocated string that must be free()'d by the caller.
 * Returns NULL on failure.
 */
char* self_exe_dup() {
    // 1. Start with a sensible static-sized buffer (Stack)
    char stack_buf[1024];
    size_t size = sizeof(stack_buf);

    // Call our internal logic
    size_t needed = get_self_exe_path(stack_buf, size);

    if (needed == 0)
        return NULL;

    // 2. If it fit in the static buffer, strdup it
    if (needed < size) {
        return strdup(stack_buf);
    }

    // 3. Otherwise, allocate the "needed" size and try again
    // We use needed + 1 to ensure space for the null terminator
    char* heap_buf = (char*)malloc(needed + 1);
    if (!heap_buf)
        return NULL;

    size_t final_len = get_self_exe_path(heap_buf, needed + 1);

    if (final_len == 0 || final_len > needed) {
        free(heap_buf);
        return NULL;
    }

    return heap_buf;
}

static char* g_self_exe = NULL;

static void __attribute__((destructor)) free_self_exe() {
    if (g_self_exe) {
        // printf("freeing self exe: %s\n", g_self_exe);
        free(g_self_exe);
        g_self_exe = NULL;
    }
}

#ifdef _WIN32
// Force the linker to put a pointer to this function in a specific CRT section
// "XPU" is for "Uninitialized" or late-stage termination
#pragma section(".CRT$XPU", read)
__declspec(allocate(".CRT$XPU")) //
static void (*p_free_self_exe)(void) = free_self_exe;

#else
__attribute__((destructor)) //
static void
p_free_self_exe(void) {
    free_self_exe();
}
#endif

const char* self_exe() {
    if (!g_self_exe) {
        g_self_exe = self_exe_dup();
    }
    return g_self_exe;
}
