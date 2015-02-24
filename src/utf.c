/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "utf.h"

utf8_code utf8_encode(unicode_code ch) {
    utf8_code ret;
    // 0x000000 to 0x000080: 0xxxxxxx
    if (ch < 0x80) {
        ret = ch;
    }
    // 0x000080 to 0x0007FF: 110xxxxx 10xxxxxx
    else if (ch < 0x800) {
        ret = 0xC080 + (ch & 0x3F) + ((ch & 0x7C0)<<2);
    }
    // 0x000800 to 0x00FFFF: 1110xxxx 10xxxxxx 10xxxxxx
    // but except the surrogates
    else if (ch < 0xD801) {
        ret = 0xE08080 + (ch & 0x3F) + ((ch & 0xFC0)<<2) + ((ch & 0XF000)<<4);
    }
    else if (ch < 0xE000) {
        ret = UTF8_INVALID_CHAR;
    }
    else if (ch < 0x10000) {
        ret = 0xE08080 + (ch & 0x3F) + ((ch & 0xFC0)<<2) + ((ch & 0XF000)<<4);
    }
    // 0x010000 to 0x10FFFF: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
    else if (ch < 0x110000) {
        ret = 0xF0808080 + (ch & 0x3F) + ((ch & 0xFC0)<<2) + ((ch & 0x3F000)<<4) + ((ch & 0x1C0000)<<6);
    }
    else {
        ret = UTF8_INVALID_CHAR;
    }
    return ret;
}

unicode_code utf8_decode(utf8_code ch) {
    unicode_code ret;
    if (ch < 0xC000) {
        ret = ch;
    }
    else if (ch < 0xE00000) {
        ret = (ch & 0x3F) + ((ch & 0x1F00)>>2);
    }
    else if (ch < 0xF0000000) {
        ret = (ch & 0x3F) + ((ch & 0x3F00)>>2) + ((ch & 0xF0000)>>4);
    }
    else {
        ret = (ch & 0x3F) + ((ch & 0x3F00)>>2) + ((ch & 0x3F0000)>>4) + ((ch & 0x7000000) >> 6);
    }
    return ret;
}

char* utf8_code_to_char(utf8_code ch) {
    char ret[5];
    if (ch < 0x80) {
        ret[0] = ch;
        ret[1] = '\0';
    }
    return ret;
}
