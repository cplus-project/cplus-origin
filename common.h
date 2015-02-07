/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     This file defines some common typedef and offers
 * the uniform error handling.
 **/

#ifndef CPLUS_COMMON_H
#define CPLUS_COMMON_H

#include <stdio.h>

#define true   1
#define false -1
typedef char                   bool;
typedef char                   byte;
typedef char                   int8;
typedef short int              int16;
typedef long int               int32;
typedef long long int          int64;
typedef unsigned char          uint8;
typedef unsigned short int     uint16;
typedef unsigned long int      uint32;
typedef unsigned long long int uint64;
typedef float                  float32;
typedef double                 float64;
typedef unsigned char          uchar;

// all error in this compiler program will be generated by
// new_error(char* errmsg).
//
// when you define a function witch may generate some error,
// you should write the definition like below:
//     error foo(bool flag) {...}
//
// when you call the function and want to check the error,
// you can judge like this:
// return value ->     NULL means none error occurs
//                 not NULL means some error occurs
typedef char* error;
extern error new_error(char* errmsg);
extern void  debug    (char* msg);

#endif
