/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     The dynamicarr.h and dynamicarr.c define and implement
 * the dynamic array of some different type.
 **/

#ifndef CPLUS_DYNAMICARR_H
#define CPLUS_DYNAMICARR_H

#include "common.h"

// this struct is used as the node of the char-type
// dynamic array.
typedef struct dynamicarr_char_node{
    char*  arr;
    uint64 cap;
    uint64 i;
    struct dynamicarr_char_node* next;
}dynamicarr_char_node;

// this is the char-type dynamic array. it can increase
// its capacity automatically. when being full, the array
// will extend its total capacity to twice than current.
typedef struct {
    uint64 total_cap;             // the total capacity of the dynamic array
    uint64 used;                  // the space used yet of the dynamic array
    dynamicarr_char_node* first;  // point to the first buffer of the dynamic array
    dynamicarr_char_node* cur;    // point to the buffer which the index is on focus now
}dynamicarr_char;

extern error dynamicarr_char_init   (dynamicarr_char* darr, uint64 capacity);
extern void  dynamicarr_char_append (dynamicarr_char* darr, char* str, uint64 len);
extern void  dynamicarr_char_appendc(dynamicarr_char* darr, char ch);
extern bool  dynamicarr_char_equal  (dynamicarr_char* darr, char* str, uint64 len);
extern char* dynamicarr_char_getstr (dynamicarr_char* darr);
extern void  dynamicarr_char_clear  (dynamicarr_char* darr);
extern void  dynamicarr_char_destroy(dynamicarr_char* darr);
extern void  dynamicarr_char_debug  (dynamicarr_char* darr);

#endif
