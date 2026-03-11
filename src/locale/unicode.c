#include "unicode.h"

#include <unicode/ucnv.h>
#include <unicode/ucnv_err.h>
#include <unicode/ustring.h>

#include <stdlib.h>
#include <string.h>

int utf8_decode_code_point(const uint8_t* ptr, const uint8_t* end, int* bytes_consumed) {
    if (bytes_consumed) *bytes_consumed = 0;
    if (!ptr || !end || ptr >= end) return -1;

    uint8_t b0 = *ptr;
    if (b0 < 0x80) {
        if (bytes_consumed) *bytes_consumed = 1;
        return (int)b0;
    }

    if ((b0 & 0xE0) == 0xC0) {
        if (ptr + 1 >= end) { if (bytes_consumed) *bytes_consumed = 1; return -2; }
        uint8_t b1 = ptr[1];
        if ((b1 & 0xC0) != 0x80) { if (bytes_consumed) *bytes_consumed = 1; return -2; }
        if (bytes_consumed) *bytes_consumed = 2;
        return (int)(((b0 & 0x1F) << 6) | (b1 & 0x3F));
    }

    if ((b0 & 0xF0) == 0xE0) {
        if (ptr + 2 >= end) { if (bytes_consumed) *bytes_consumed = 1; return -2; }
        uint8_t b1 = ptr[1], b2 = ptr[2];
        if ((b1 & 0xC0) != 0x80 || (b2 & 0xC0) != 0x80) { if (bytes_consumed) *bytes_consumed = 1; return -2; }
        if (bytes_consumed) *bytes_consumed = 3;
        return (int)(((b0 & 0x0F) << 12) | ((b1 & 0x3F) << 6) | (b2 & 0x3F));
    }

    if ((b0 & 0xF8) == 0xF0) {
        if (ptr + 3 >= end) { if (bytes_consumed) *bytes_consumed = 1; return -2; }
        uint8_t b1 = ptr[1], b2 = ptr[2], b3 = ptr[3];
        if ((b1 & 0xC0) != 0x80 || (b2 & 0xC0) != 0x80 || (b3 & 0xC0) != 0x80) { if (bytes_consumed) *bytes_consumed = 1; return -2; }
        if (bytes_consumed) *bytes_consumed = 4;
        return (int)(((b0 & 0x07) << 18) | ((b1 & 0x3F) << 12) | ((b2 & 0x3F) << 6) | (b3 & 0x3F));
    }

    if (bytes_consumed) *bytes_consumed = 1;
    return -2;
}

int utf8_encode_code_point(int cp, uint8_t out[4], int* out_len) {
    if (out_len) *out_len = 0;
    if (!out) return 0;
    if (cp < 0 || cp > 0x10FFFF) return 0;

    if (cp < 0x80) {
        out[0] = (uint8_t)cp;
        if (out_len) *out_len = 1;
        return 1;
    } else if (cp < 0x800) {
        out[0] = (uint8_t)(0xC0 | (cp >> 6));
        out[1] = (uint8_t)(0x80 | (cp & 0x3F));
        if (out_len) *out_len = 2;
        return 1;
    } else if (cp < 0x10000) {
        out[0] = (uint8_t)(0xE0 | (cp >> 12));
        out[1] = (uint8_t)(0x80 | ((cp >> 6) & 0x3F));
        out[2] = (uint8_t)(0x80 | (cp & 0x3F));
        if (out_len) *out_len = 3;
        return 1;
    } else {
        out[0] = (uint8_t)(0xF0 | (cp >> 18));
        out[1] = (uint8_t)(0x80 | ((cp >> 12) & 0x3F));
        out[2] = (uint8_t)(0x80 | ((cp >> 6) & 0x3F));
        out[3] = (uint8_t)(0x80 | (cp & 0x3F));
        if (out_len) *out_len = 4;
        return 1;
    }
}

static void set_to_u_callback(UConverter* conv, unicode_conversion_mode mode, UErrorCode* status) {
    switch (mode) {
        case UNICODE_SKIP:
            ucnv_setToUCallBack(conv, UCNV_TO_U_CALLBACK_SKIP, NULL, NULL, NULL, status);
            break;
        case UNICODE_REPLACE:
            ucnv_setToUCallBack(conv, UCNV_TO_U_CALLBACK_SUBSTITUTE, NULL, NULL, NULL, status);
            break;
        case UNICODE_ERROR:
        default:
            ucnv_setToUCallBack(conv, UCNV_TO_U_CALLBACK_STOP, NULL, NULL, NULL, status);
            break;
    }
}

u16_string u16_from_encoding(const char* data, int32_t len,
                                     const char* charset,
                                     unicode_conversion_mode mode,
                                     UChar32 replacement,
                                     UErrorCode* status)
{
    (void)replacement; /* currently not wired: matches previous behavior */
    u16_string out;
    out.data = NULL;
    out.length = 0;

    if (status) *status = U_ZERO_ERROR;
    if (!data || len <= 0 || !charset || charset[0] == '\0') {
        if (status) *status = U_ILLEGAL_ARGUMENT_ERROR;
        return out;
    }

    UErrorCode st = U_ZERO_ERROR;
    UConverter* conv = ucnv_open(charset, &st);
    if (U_FAILURE(st)) {
        if (status) *status = st;
        return out;
    }

    set_to_u_callback(conv, mode, &st);
    if (U_FAILURE(st)) {
        ucnv_close(conv);
        if (status) *status = st;
        return out;
    }

    /* Preflight */
    int32_t needed = ucnv_toUChars(conv, NULL, 0, data, len, &st);
    if (st != U_BUFFER_OVERFLOW_ERROR && U_FAILURE(st)) {
        ucnv_close(conv);
        if (status) *status = st;
        return out;
    }
    st = U_ZERO_ERROR;

    UChar* buf = (UChar*)malloc((size_t)(needed + 1) * sizeof(UChar));
    if (!buf) {
        ucnv_close(conv);
        if (status) *status = U_MEMORY_ALLOCATION_ERROR;
        return out;
    }

    int32_t actual = ucnv_toUChars(conv, buf, needed + 1, data, len, &st);
    ucnv_close(conv);

    if (U_FAILURE(st)) {
        free(buf);
        if (status) *status = st;
        return out;
    }

    out.data = buf;
    out.length = actual;
    if (status) *status = st;
    return out;
}

void u16_free(u16_string* s) {
    if (!s) return;
    if (s->data) free(s->data);
    s->data = NULL;
    s->length = 0;
}

char* u16_to_utf8(const UChar* data, int32_t len,
                      int32_t* out_len,
                      UErrorCode* status)
{
    if (out_len) *out_len = 0;
    if (status) *status = U_ZERO_ERROR;
    if (!data || len < 0) {
        if (status) *status = U_ILLEGAL_ARGUMENT_ERROR;
        return NULL;
    }

    UErrorCode st = U_ZERO_ERROR;
    int32_t needed = 0;
    u_strToUTF8(NULL, 0, &needed, data, len, &st);
    if (st != U_BUFFER_OVERFLOW_ERROR && U_FAILURE(st)) {
        if (status) *status = st;
        return NULL;
    }
    st = U_ZERO_ERROR;

    char* buf = (char*)malloc((size_t)needed + 1);
    if (!buf) {
        if (status) *status = U_MEMORY_ALLOCATION_ERROR;
        return NULL;
    }
    u_strToUTF8(buf, needed + 1, &needed, data, len, &st);
    if (U_FAILURE(st)) {
        free(buf);
        if (status) *status = st;
        return NULL;
    }
    buf[needed] = '\0';
    if (out_len) *out_len = needed;
    if (status) *status = st;
    return buf;
}

UChar32* u16_to_utf32(const UChar* data, int32_t len,
                          int32_t* out_len,
                          UErrorCode* status)
{
    if (out_len) *out_len = 0;
    if (status) *status = U_ZERO_ERROR;
    if (!data || len < 0) {
        if (status) *status = U_ILLEGAL_ARGUMENT_ERROR;
        return NULL;
    }

    UErrorCode st = U_ZERO_ERROR;
    int32_t needed = 0;
    u_strToUTF32(NULL, 0, &needed, data, len, &st);
    if (st != U_BUFFER_OVERFLOW_ERROR && U_FAILURE(st)) {
        if (status) *status = st;
        return NULL;
    }
    st = U_ZERO_ERROR;

    UChar32* buf = (UChar32*)malloc((size_t)needed * sizeof(UChar32));
    if (!buf) {
        if (status) *status = U_MEMORY_ALLOCATION_ERROR;
        return NULL;
    }
    u_strToUTF32(buf, needed, &needed, data, len, &st);
    if (U_FAILURE(st)) {
        free(buf);
        if (status) *status = st;
        return NULL;
    }
    if (out_len) *out_len = needed;
    if (status) *status = st;
    return buf;
}

uint8_t* u16_to_encoding(const UChar* data, int32_t len,
                             const char* charset,
                             int32_t* out_len,
                             UErrorCode* status)
{
    if (out_len) *out_len = 0;
    if (status) *status = U_ZERO_ERROR;
    if (!data || len < 0 || !charset || charset[0] == '\0') {
        if (status) *status = U_ILLEGAL_ARGUMENT_ERROR;
        return NULL;
    }

    UErrorCode st = U_ZERO_ERROR;
    UConverter* conv = ucnv_open(charset, &st);
    if (U_FAILURE(st)) {
        if (status) *status = st;
        return NULL;
    }

    /* Preflight */
    int32_t needed = ucnv_fromUChars(conv, NULL, 0, data, len, &st);
    if (st != U_BUFFER_OVERFLOW_ERROR && U_FAILURE(st)) {
        ucnv_close(conv);
        if (status) *status = st;
        return NULL;
    }
    st = U_ZERO_ERROR;

    char* buf = (char*)malloc((size_t)needed);
    if (!buf) {
        ucnv_close(conv);
        if (status) *status = U_MEMORY_ALLOCATION_ERROR;
        return NULL;
    }

    int32_t actual = ucnv_fromUChars(conv, buf, needed, data, len, &st);
    ucnv_close(conv);
    if (U_FAILURE(st)) {
        free(buf);
        if (status) *status = st;
        return NULL;
    }

    if (out_len) *out_len = actual;
    if (status) *status = st;
    return (uint8_t*)buf;
}

