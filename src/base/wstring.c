#include "wstring.h"

#include <stdlib.h>
#include <string.h>

size_t wstring_len(const uint16_t* str) {
    if (!str) return 0;
    const uint16_t* p = str;
    while (*p) p++;
    return (size_t)(p - str);
}

int wstring_cmp(const uint16_t* a, const uint16_t* b) {
    if (!a) return b ? -1 : 0;
    if (!b) return 1;
    while (*a && *b && *a == *b) { a++; b++; }
    return (int)*a - (int)*b;
}

const uint16_t* wstring_strstr(const uint16_t* haystack, const uint16_t* needle) {
    if (!haystack || !needle || !*needle) return haystack;
    size_t nlen = wstring_len(needle);
    for (; *haystack; haystack++) {
        size_t i = 0;
        for (; i < nlen && haystack[i] == needle[i]; i++) ;
        if (i == nlen) return haystack;
    }
    return NULL;
}

uint16_t* wstring_dup(const uint16_t* str) {
    if (!str) return NULL;
    size_t n = wstring_len(str) + 1;
    uint16_t* p = (uint16_t*)malloc(n * sizeof(uint16_t));
    if (!p) return NULL;
    memcpy(p, str, n * sizeof(uint16_t));
    return p;
}

void wstring_from_ascii(const char* ascii, uint16_t* wide, size_t max_len) {
    if (!ascii || !wide || max_len == 0) return;
    size_t i = 0;
    while (i + 1 < max_len && ascii[i]) {
        wide[i] = (uint16_t)(unsigned char)ascii[i];
        i++;
    }
    wide[i] = 0;
}
