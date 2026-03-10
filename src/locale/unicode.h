#ifndef UTIL_UNICODE_H
#define UTIL_UNICODE_H

#include <stdint.h>
#include <stddef.h>

#include <unicode/utypes.h>   /* UChar, UChar32, UErrorCode */

#ifdef __cplusplus
extern "C" {
#endif

typedef enum unicode_conversion_mode {
    UNICODE_SKIP = 0,
    UNICODE_REPLACE = 1,
    UNICODE_ERROR = 2,
} unicode_conversion_mode;

typedef struct u16_string {
    int32_t length;   /* number of UChar code units */
    UChar* data;      /* malloc-owned UTF-16 buffer (not necessarily NUL-terminated) */
} u16_string;

/* Decode one UTF-8 code point at [ptr, end). Returns codepoint.
 * - EOF: returns -1, bytes_consumed=0
 * - malformed: returns -2, bytes_consumed>0 (best-effort)
 */
int utf8_decode_code_point(const uint8_t* ptr, const uint8_t* end, int* bytes_consumed);

/* Encode one Unicode code point as UTF-8.
 * Returns 1 on success (out_len set to 1..4), 0 on invalid cp (out_len=0).
 */
int utf8_encode_code_point(int cp, uint8_t out[4], int* out_len);

/* Convert bytes in charset -> UTF-16 (UChar). Returned buffer is malloc-owned. */
u16_string u16_from_encoding(const char* data, int32_t len,
                                     const char* charset,
                                     unicode_conversion_mode mode,
                                     UChar32 replacement,
                                     UErrorCode* status);

/* Free a u16_string and reset it to empty. */
void u16_free(u16_string* s);

/* UTF-16 (UChar) -> UTF-8. Returns malloc-owned buffer, NUL-terminated. */
char* u16_to_utf8(const UChar* data, int32_t len,
                      int32_t* out_len,
                      UErrorCode* status);

/* UTF-16 (UChar) -> UTF-32. Returns malloc-owned buffer. */
UChar32* u16_to_utf32(const UChar* data, int32_t len,
                          int32_t* out_len,
                          UErrorCode* status);

/* UTF-16 (UChar) -> encoded bytes (charset). Returns malloc-owned buffer. */
uint8_t* u16_to_encoding(const UChar* data, int32_t len,
                             const char* charset,
                             int32_t* out_len,
                             UErrorCode* status);

#ifdef __cplusplus
}
#endif

#endif /* UTIL_UNICODE_H */
