/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "utf.h"

// this function is often used together with utf8_char_to_code.
// because when you want to get a utf8_code from the char
// stream, you should know the the number of the bytes which
// the current position's rune contains. you can call this
// function to get the number.
uint8 utf8_calcu_bytes(char ch) {
    if (ch < 0x80) {
        return 1;
    }
    else if (0xC0 <= ch && ch < 0xE0) {
        return 2;
    }
    else if (0xE0 <= ch && ch < 0xF0) {
        return 3;
    }
    else if (ch >= 0xF0) {
        return 4;
    }
    return 0;
}

utf8_code utf8_encode(unicode_code code) {
    utf8_code ret;
    // 0x000000 to 0x000080: 0xxxxxxx
    if (code < 0x80) {
        ret = code;
    }
    // 0x000080 to 0x0007FF: 110xxxxx 10xxxxxx
    else if (code < 0x800) {
        ret = 0xC080 + (code & 0x3F) + ((code & 0x7C0)<<2);
    }
    // 0x000800 to 0x00FFFF: 1110xxxx 10xxxxxx 10xxxxxx
    // but except the surrogates
    else if (code < 0xD801) {
        ret = 0xE08080 + (code & 0x3F) + ((code & 0xFC0)<<2) + ((code & 0XF000)<<4);
    }
    else if (code < 0xE000) {
        ret = UTF8_INVALID_CODE;
    }
    else if (code < 0x10000) {
        ret = 0xE08080 + (code & 0x3F) + ((code & 0xFC0)<<2) + ((code & 0XF000)<<4);
    }
    // 0x010000 to 0x10FFFF: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
    else if (code < 0x110000) {
        ret = 0xF0808080 + (code & 0x3F) + ((code & 0xFC0)<<2) + ((code & 0x3F000)<<4) + ((code & 0x1C0000)<<6);
    }
    else {
        ret = UTF8_INVALID_CODE;
    }
    return ret;
}

unicode_code utf8_decode(utf8_code code) {
    unicode_code ret;
    if (code < 0xC000) {
        ret = code;
    }
    else if (code < 0xE00000) {
        ret = (code & 0x3F) + ((code & 0x1F00)>>2);
    }
    else if (code < 0xF0000000) {
        ret = (code & 0x3F) + ((code & 0x3F00)>>2) + ((code & 0xF0000)>>4);
    }
    else {
        ret = (code & 0x3F) + ((code & 0x3F00)>>2) + ((code & 0x3F0000)>>4) + ((code & 0x7000000) >> 6);
    }
    return ret;
}

utf8_char utf8_code_to_char(utf8_code code) {
    utf8_char ret;
    if (code < 0x80) {
        ret.bytes = 1;
        ret.utf8_bytes.utf8_bytes_1 = code;
    }
    else if (0xC080 <= code && code < 0xE08080) {
        ret.bytes = 2;
        ret.utf8_bytes.utf8_bytes_2[0] = (code & 0xFF00) >> 8;
        ret.utf8_bytes.utf8_bytes_2[1] =  code & 0x00FF;
    }
    else if (0xE08080 <= code && code < 0xF0808080) {
        ret.bytes = 3;
        ret.utf8_bytes.utf8_bytes_3[0] = (code & 0xFF0000) >> 16;
        ret.utf8_bytes.utf8_bytes_3[1] = (code & 0x00FF00) >> 8;
        ret.utf8_bytes.utf8_bytes_3[2] =  code & 0x0000FF;
    }
    else if (code >= 0xF0808080) {
        ret.bytes = 4;
        ret.utf8_bytes.utf8_bytes_4[0] = (code & 0xFF000000) >> 24;
        ret.utf8_bytes.utf8_bytes_4[1] = (code & 0x00FF0000) >> 16;
        ret.utf8_bytes.utf8_bytes_4[2] = (code & 0x0000FF00) >> 8;
        ret.utf8_bytes.utf8_bytes_4[3] =  code & 0x000000FF;
    }
    else {
        ret.bytes = 0;
    }
    return ret;
}

// maybe you will need the help of the utf8_calcu_bytes.
utf8_code utf8_char_to_code(utf8_char ch) {
    utf8_code ret = 0;
    switch (ch.bytes) {
    case 1:
        return ch.utf8_bytes.utf8_bytes_1;
    case 2:
        ret = ret | (ch.utf8_bytes.utf8_bytes_2[0] << 8);
        ret = ret |  ch.utf8_bytes.utf8_bytes_2[1];
        return ret;
    case 3:
        ret = ret | (ch.utf8_bytes.utf8_bytes_3[0] << 16);
        ret = ret | (ch.utf8_bytes.utf8_bytes_3[1] << 8);
        ret = ret |  ch.utf8_bytes.utf8_bytes_3[2];
        return ret;
    case 4:
        ret = ret | (ch.utf8_bytes.utf8_bytes_4[0] << 24);
        ret = ret | (ch.utf8_bytes.utf8_bytes_4[1] << 16);
        ret = ret | (ch.utf8_bytes.utf8_bytes_4[2] << 8);
        ret = ret |  ch.utf8_bytes.utf8_bytes_4[3];
        return ret;
    default:
        return UTF8_INVALID_CODE;
    }
}

// print one UTF-8 rune on the screen.
void utf8_print(utf8_char ch) {
    switch (ch.bytes) {
    case 1:
        printf("%c", ch.utf8_bytes.utf8_bytes_1);
        break;
    case 2:
        printf("%s", ch.utf8_bytes.utf8_bytes_2);
        break;
    case 3:
        printf("%s", ch.utf8_bytes.utf8_bytes_3);
        break;
    case 4:
        printf("%s", ch.utf8_bytes.utf8_bytes_4);
        break;
    default:
        debug("invalid UTF-8 char");
    }
}
