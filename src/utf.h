/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     The utf.h and utf.c provide the UTF(Unicode
 * Translation Format) support.
 **/

#ifndef CPLUS_UTF_H
#define CPLUS_UTF_H

#include "common.h"
#include "dynamicarr.h"

#define UTF8_INVALID_CODE 0xFF000000

typedef unsigned long int unicode_code;
typedef unsigned long int utf8_code;

// the utf8_char is used to represent one utf-8 rune.
// for example:
//   one utf8_code 0x654E3F will be converted like this:
//   utf8_char rune;
//   rune.utf8_bytes = 3;
//   rune.utf8_bytes.utf8_bytes_3[0] = 0x65;
//   rune.utf8_bytes.utf8_bytes_3[1] = 0x4E;
//   rune.utf8_bytes.utf8_bytes_3[2] = 0x3F;
typedef struct {
    uint8 bytes; // the number of the bytes which the UTF-8 char contains
    union {
        char utf8_bytes_1;    // when bytes = 1
        char utf8_bytes_2[2]; // when bytes = 2
        char utf8_bytes_3[3]; // when bytes = 3
        char utf8_bytes_4[4]; // when bytes = 4
    }utf8_bytes;
}utf8_char;

extern uint8        utf8_calcu_bytes (char ch);
extern utf8_code    utf8_encode      (unicode_code code);
extern unicode_code utf8_decode      (utf8_code code);
extern utf8_char    utf8_code_to_char(utf8_code code);
extern utf8_code    utf8_char_to_code(utf8_char ch);
extern void         utf8_print       (utf8_char ch);

#endif
