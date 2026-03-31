#ifndef __FILE_TYPE_H
#define __FILE_TYPE_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Detect file type from header bytes and filename
 * @param path File path (used for extension detection)
 * @param header First bytes of the file
 * @param header_len Length of header bytes
 * @return Newly allocated string describing file type (caller must free)
 */
char* detect_file_type(const char* path, const uint8_t* header, size_t header_len);

#ifdef __cplusplus
}
#endif

#endif // __FILE_TYPE_H

