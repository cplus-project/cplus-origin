/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     This file defines some common typedefs, offers
 * the uniform error handling and memory operations.
 **/

#ifndef CPLUS_COMMON_H
#define CPLUS_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <malloc.h>

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


#define ERROR_TYPE_GENERAL   0x00
#define ERROR_TYPE_LEXICAL   0x01
#define ERROR_TYPE_SYNTACTIC 0x02
#define ERROR_TYPE_SEMANTIC  0x03

typedef struct error_list_node {
    error err;
    int32 err_type;
    int32 line_count;
    int16 line_pos;
    struct error_list_node* next;
}error_list_node;

// the error_list is used to store a set of compile-error.
// it is different from the another error of C+ language.
typedef struct {
    int8 err_count;
    int8 upper_limit;
    error_list_node* head;
    error_list_node* tail;
}error_list;

extern void error_list_init     (error_list* errlist, int8 upper_limit);
extern bool error_list_add      (error_list* errlist, error err, int32 err_type, int32 line_count, int16 line_pos);
extern int8 error_list_err_count(error_list* errlist);
extern void error_list_display  (error_list* errlist);
extern void error_list_destroy  (error_list* errlist);

// all operations about memory allocating/releasing must use
// the function mem_alloc and mem_free. mem_alloc can process
// the error automatically. mem_free will work well even
// though you free the same memory many times.
extern void* mem_alloc(size_t size);
extern void  mem_free (void *ptr);

// show some message to debug.
extern void  debug(char* msg);

#endif
