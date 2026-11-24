#include <stdint.h>
#include <stdlib.h>
#include <uchar.h>

int64_t _ZN14TrafficControl15connectToServeeEb(uint8_t value __unused) {
    return -1;
}

static uint32_t utf8_next_char(const uint8_t** p) {
    uint8_t c = **p;
    if (c < 0x80) {
        (*p)++;
        return c;
    }
    uint32_t cp = 0;
    int extra = 0;
    if ((c & 0xE0) == 0xC0) {
        cp = c & 0x1F;
        extra = 1;
    } else if ((c & 0xF0) == 0xE0) {
        cp = c & 0x0F;
        extra = 2;
    } else if ((c & 0xF8) == 0xF0) {
        cp = c & 0x07;
        extra = 3;
    } else {
        (*p)++;
        return 0xFFFD;
    }
    (*p)++;
    for (int i = 0; i < extra; i++) {
        uint8_t cont = **p;
        if ((cont & 0xC0) != 0x80) {
            return 0xFFFD;
        }
        cp = (cp << 6) | (cont & 0x3F);
        (*p)++;
    }
    return cp;
}

char16_t* strdup8to16(const char* s, size_t* out_len) {
    if (s == NULL) {
        if (out_len) *out_len = 0;
        return NULL;
    }

    size_t len = 0;
    const uint8_t* p = (const uint8_t*)s;
    while (*p != '\0') {
        uint32_t cp = utf8_next_char(&p);
        len += (cp > 0xFFFF) ? 2 : 1;
    }

    char16_t* ret = (char16_t*)malloc((len + 1) * sizeof(char16_t));
    if (ret == NULL) {
        if (out_len) *out_len = 0;
        return NULL;
    }

    p = (const uint8_t*)s;
    size_t idx = 0;
    while (*p != '\0') {
        uint32_t cp = utf8_next_char(&p);
        if (cp > 0xFFFF) {
            cp -= 0x10000;
            ret[idx++] = (char16_t)(0xD800 + (cp >> 10));
            ret[idx++] = (char16_t)(0xDC00 + (cp & 0x3FF));
        } else {
            ret[idx++] = (char16_t)cp;
        }
    }
    ret[idx] = 0;

    if (out_len) *out_len = idx;
    return ret;
}
