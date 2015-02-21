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

typedef unsigned long int utf_char;

extern bool  utf8_check_valid      (utf_char ch);
extern error utf8_encode           (utf_char ch);
extern error utf8_decode           (utf_char ch);
extern error utf8_encode_dynamicarr(dynamicarr_char* darr);
extern error utf8_decode_dynamicarr(dynamicarr_char* darr);

#endif
