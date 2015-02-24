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

#define UTF8_INVALID_CHAR 0xFF000000

typedef unsigned long int unicode_code;

typedef unsigned long int utf8_code;
extern utf8_code    utf8_encode      (unicode_code ch);
extern unicode_code utf8_decode      (utf8_code ch);
extern char*        utf8_code_to_char(utf8_code ch);

#endif
