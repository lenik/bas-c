#include "string_list.h"

#include <stdlib.h>
#include <string.h>

string_list* string_list_create(size_t initial_capacity) {
    string_list* list = (string_list*)malloc(sizeof(string_list));
    if (!list) return NULL;
    list->count = 0;
    list->capacity = initial_capacity > 0 ? initial_capacity : 1;
    list->items = (char**)malloc(list->capacity * sizeof(char*));
    if (!list->items) {
        free(list);
        return NULL;
    }
    return list;
}

void string_list_free(string_list* list) {
    if (!list) return;
    if (list->items) {
        for (size_t i = 0; i < list->count; i++)
            free(list->items[i]);
        free(list->items);
    }
    free(list);
}

static int ensure_capacity(string_list* list, size_t need) {
    if (need <= list->capacity) return 1;
    size_t new_cap = list->capacity;
    while (new_cap < need) new_cap *= 2;
    char** p = (char**)realloc(list->items, new_cap * sizeof(char*));
    if (!p) return 0;
    list->items = p;
    list->capacity = new_cap;
    return 1;
}

int string_list_add(string_list* list, const char* str) {
    if (!list || !str) return 0;
    if (!ensure_capacity(list, list->count + 1)) return 0;
    char* copy = strdup(str);
    if (!copy) return 0;
    list->items[list->count++] = copy;
    return 1;
}

int string_list_append(string_list* list, const char* str) {
    return string_list_add(list, str);
}

int string_list_insert(string_list* list, size_t index, const char* str) {
    if (!list || !str || index > list->count) return 0;
    if (!ensure_capacity(list, list->count + 1)) return 0;
    char* copy = strdup(str);
    if (!copy) return 0;
    memmove(list->items + index + 1, list->items + index,
            (list->count - index) * sizeof(char*));
    list->items[index] = copy;
    list->count++;
    return 1;
}

int string_list_remove(string_list* list, size_t index) {
    if (!list || index >= list->count) return 0;
    free(list->items[index]);
    memmove(list->items + index, list->items + index + 1,
            (list->count - 1 - index) * sizeof(char*));
    list->count--;
    return 1;
}

const char* string_list_get(const string_list* list, size_t index) {
    if (!list || index >= list->count) return NULL;
    return list->items[index];
}

ssize_t string_list_find(const string_list* list, const char* str) {
    if (!list || !str) return -1;
    for (size_t i = 0; i < list->count; i++)
        if (strcmp(list->items[i], str) == 0) return (ssize_t)i;
    return -1;
}

int string_list_contains(const string_list* list, const char* str) {
    return string_list_find(list, str) >= 0;
}

size_t string_list_size(const string_list* list) {
    return list ? list->count : 0;
}

size_t string_list_capacity(const string_list* list) {
    return list ? list->capacity : 0;
}

void string_list_clear(string_list* list) {
    if (!list) return;
    for (size_t i = 0; i < list->count; i++) free(list->items[i]);
    list->count = 0;
}

int string_list_resize(string_list* list, size_t new_capacity) {
    if (!list || new_capacity < list->count) return 0;
    char** p = (char**)realloc(list->items, new_capacity * sizeof(char*));
    if (!p) return 0;
    list->items = p;
    list->capacity = new_capacity;
    return 1;
}

static int str_compare(const void* a, const void* b) {
    return strcmp(*(const char* const*)a, *(const char* const*)b);
}

void string_list_sort(string_list* list) {
    if (!list || list->count < 2) return;
    qsort(list->items, list->count, sizeof(char*), str_compare);
}

char* string_list_join(const string_list* list, const char* separator) {
    if (!list || list->count == 0) return strdup("");
    if (!separator) separator = "";
    size_t sep_len = strlen(separator);
    size_t total = 0;
    for (size_t i = 0; i < list->count; i++)
        total += strlen(list->items[i]);
    total += (list->count - 1) * sep_len + 1;
    char* out = (char*)malloc(total);
    if (!out) return NULL;
    char* p = out;
    for (size_t i = 0; i < list->count; i++) {
        if (i) { memcpy(p, separator, sep_len + 1); p += sep_len; }
        size_t n = strlen(list->items[i]);
        memcpy(p, list->items[i], n + 1);
        p += n;
    }
    return out;
}

string_list* string_list_copy(const string_list* list) {
    if (!list) return NULL;
    string_list* c = string_list_create(list->capacity);
    if (!c) return NULL;
    for (size_t i = 0; i < list->count; i++)
        if (!string_list_add(c, list->items[i])) {
            string_list_free(c);
            return NULL;
        }
    return c;
}

void string_list_unique(string_list* list) {
    if (!list || list->count < 2) return;
    size_t w = 1;
    for (size_t r = 1; r < list->count; r++) {
        if (strcmp(list->items[r], list->items[w - 1]) != 0) {
            if (w != r) list->items[w] = list->items[r];
            w++;
        } else {
            free(list->items[r]);
        }
    }
    list->count = w;
}
