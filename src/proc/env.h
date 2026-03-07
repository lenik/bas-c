#ifndef __PROC__ENV_H
#define __PROC__ENV_H

#include <stdio.h>

/* Find the name in PATH environ. Return the absolute pathname if the name is
   found in directory denoted by PATH. Otherwise return NULL.

   The returned string should be freed after use. */
char *path_find(const char *name);

/* The combination of path_find and path_normalize.

   The returned string should be freed after use.*/
char *path_find_norm(const char *name);

/**
 * Securely creates a temporary file.
 * @path_out: Buffer of size MAX_PATH to store the resulting filename
 * @path_out_size: Size of the path_out buffer, including the null terminator
 * @templ: Template string, default is "XXXXXX". Can be absolute or relative path,
   if relative, it will be joined with the tmp_dir or environment variable.
   @tmp_dir: Temporary directory, default is /tmp or /var/tmp or user defined.
 * Returns: The absolute path to the temporary file or NULL on failure.
 */
char *etempl(char *path_out, size_t path_out_size, const char *templ, const char *tmp_dir);

#define EMKSTEMP(templ) emkstemp(templ, sizeof(templ), NULL, NULL)
#define EMKDTEMP(templ) emkdtemp(templ, sizeof(templ), NULL, NULL)
#define EMKTEMP_OPEN(templ) emktemp_open(templ, sizeof(templ), NULL, NULL, NULL)

/**
 * Securely creates a temporary file.
 * @path_out: Buffer of size MAX_PATH to store the resulting filename
 * @path_out_size: Size of the path_out buffer, including the null terminator
 * @templ: Template string, default is "XXXXXX". Can be absolute or relative path,
   if relative, it will be joined with the tmp_dir or environment variable.
   @tmp_dir: Temporary directory, default is /tmp or /var/tmp or user defined.
 * Returns: File descriptor on success, -1 on failure.
 */
 int emkstemp(char *path_out, size_t path_out_size, const char *templ, const char *tmp_dir);

/**
 * Securely creates a temporary directory.
 * @path_out: Buffer of size MAX_PATH to store the resulting filename
 * @path_out_size: Size of the path_out buffer, including the null terminator
 * @templ: Template string, default is "XXXXXX". Can be absolute or relative path,
   if relative, it will be joined with the tmp_dir or environment variable.
   @tmp_dir: Temporary directory, default is /tmp or /var/tmp or user defined.
 * Returns: The absolute path to the temporary directory or NULL on failure.
 */
char *emkdtemp(char *path_out, size_t path_out_size, const char *templ, const char *tmp_dir);

/**
 * Securely creates and opens a temporary file
 * @path_out: Buffer of size MAX_PATH to store the resulting filename
 * @path_out_size: Size of the path_out buffer, including the null terminator
 * @templ: Template string, default is "XXXXXX". Can be absolute or relative path,
   if relative, it will be joined with the tmp_dir or environment variable.
   @tmp_dir: Temporary directory, default is /tmp or /var/tmp or user defined.
 * @mode: Opening mode (e.g., "w", "wb", etc.)
 * 
 * Returns: A FILE pointer to the temporary file or NULL on failure.
 */
FILE *emktemp_open(char *path_out, size_t path_out_size, const char *templ, const char *tmp_dir, const char *mode);

#endif /* __PROC__ENV_H */
