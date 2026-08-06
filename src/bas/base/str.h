#ifndef __BAS_STR_H
#define __BAS_STR_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

bool streq(const char *a, const char *b);
char *startswith(const char *s, const char *t);
char *endswith(const char *s, const char *t);

/* Heap-allocated copy of s (ISO C99). Caller must free. NULL if s is NULL or OOM. */
char *str_dup(const char *s);

/* Growable line reader (ISO C99 stand-in for POSIX getline).
 * Returns bytes stored (incl. newline if present), 0 on EOF, -1 on error. */
long str_getline(char **lineptr, size_t *n, FILE *stream);

#define LTRIM(s) while (isspace(*(s))) (s)++
char *ltrim(const char *s);
char *rtrim(char *s);
char *trim(char *s);

char *chop(char *s);
char *chomp(char *s);

char *readtok(char **endp);
char *strtok_eol(char *head, char **endp);

/* Break the quoted string, return the head token, and advance the *endp so as
   to prepare to return the next token. */
char *qstr_btok(char *head, char **endp, bool killq);

#endif
