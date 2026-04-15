#ifndef __PROC__ENV_H
#define __PROC__ENV_H

#include <stdio.h>

/* Find the name in PATH environ. Return the absolute pathname if the name is
   found in directory denoted by PATH. Otherwise return NULL.

   The returned string should be freed after use. */
char* path_find(const char* name);

/* The combination of path_find and path_normalize.

   The returned string should be freed after use.*/
char* path_find_norm(const char* name);

/**
 * Securely creates a temporary file.
 * @path_out: Buffer of size MAX_PATH to store the resulting filename
 * @path_out_size: Size of the path_out buffer, including the null terminator
 * @templ: Template string, default is "XXXXXX". Can be absolute or relative path,
   if relative, it will be joined with the tmp_dir or environment variable.
   @tmp_dir: Temporary directory, default is /tmp or /var/tmp or user defined.
 * Returns: The absolute path to the temporary file or NULL on failure.
 */
char* etempl(char* path_out, size_t path_out_size, const char* templ, const char* tmp_dir);

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
int emkstemp(char* path_out, size_t path_out_size, const char* templ, const char* tmp_dir);

/**
 * Securely creates a temporary directory.
 * @path_out: Buffer of size MAX_PATH to store the resulting filename
 * @path_out_size: Size of the path_out buffer, including the null terminator
 * @templ: Template string, default is "XXXXXX". Can be absolute or relative path,
   if relative, it will be joined with the tmp_dir or environment variable.
   @tmp_dir: Temporary directory, default is /tmp or /var/tmp or user defined.
 * Returns: The absolute path to the temporary directory or NULL on failure.
 */
char* emkdtemp(char* path_out, size_t path_out_size, const char* templ, const char* tmp_dir);

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
FILE* emktemp_open(char* path_out, size_t path_out_size, const char* templ, const char* tmp_dir,
                   const char* mode);

/**
 * Find the path of the current executable.  (snprintf style)
 *
 * @buffer: Buffer of size to store the resulting filename
 * @size: Size of the buffer, including the null terminator
 * Returns: The size of the actual path (without the null terminator) or 0 on failure.
 */
size_t get_self_exe_path(char* buffer, size_t size);

/**
 * Get the path of the current executable.
 * Returns a heap-allocated string that must be free()'d by the caller.
 * Returns NULL on failure.
 */
char* self_exe_dup();

/**
 * Get the path of the current executable.
 * Returns a pointer to a static string.
 * Returns NULL on failure.
 */
const char* self_exe();

#endif /* __PROC__ENV_H */
