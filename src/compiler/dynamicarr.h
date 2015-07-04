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
typedef struct DynamicArrCharNode{
    char* arr;
    int64 cap;
    int64 i;
    struct DynamicArrCharNode* next;
}DynamicArrCharNode;

// this is the char-type dynamic array. it can increase
// its capacity automatically. when being full, the array
// will extend its total capacity to twice than current.
typedef struct {
    int64 total_cap;           // the total capacity of the dynamic array
    int64 used;                // the space used yet of the dynamic array
    DynamicArrCharNode* first; // point to the first buffer of the dynamic array
    DynamicArrCharNode* cur;   // point to the buffer which the index is on focus now
}DynamicArrChar;

extern error dynamicArrCharInit   (DynamicArrChar* darr, int64 capacity);
extern void  dynamicArrCharAppend (DynamicArrChar* darr, char* str, int64 len);
extern void  dynamicArrCharAppendc(DynamicArrChar* darr, char  ch);
extern bool  dynamicArrCharEqual  (DynamicArrChar* darr, char* str, int64 len);
extern char* dynamicArrCharGetStr (DynamicArrChar* darr);
extern void  dynamicArrCharClear  (DynamicArrChar* darr);
extern void  dynamicArrCharDestroy(DynamicArrChar* darr);

#endif
