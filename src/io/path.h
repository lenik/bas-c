#ifndef __BAS_PATH_H
#define __BAS_PATH_H

/* Normalize a given path. Return the normalized path if succeeded, otherwise
   NULL.

   The returned string should be freed after use. */
char *path_normalize(const char *path);

#endif
