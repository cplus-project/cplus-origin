/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     The errors.h and errors.c implement a red black tree
 * to save the error tags and provide some operations to
 * match and find an error or a set of errors quickly.
 **/

#ifndef CPLUS_ERRORS_H
#define CPLUS_ERRORS_H

#include "common.h"

#define NODE_COLOR_RED   0x00
#define NODE_COLOR_BLACK 0x01

#define NODE_CMP_LT      0x00
#define NODE_CMP_EQ      0x01
#define NODE_CMP_GT      0x02

typedef struct error_table_node {
    char* errtag;
    bool  must;  // if must is ture, the error must be dealt in time
    int8  color; // NODE_COLOR_RED or NODE_COLOR_BLACK
    struct error_table_node* parent;
    struct error_table_node* lchild;
    struct error_table_node* rchild;
}error_table_node;

// error table is implemented with the rbtree data-struct,
// the search performance will be always not bad for most
// situations.
typedef struct {
    error_table_node* root;
    int32             must_count; // the number of the errors whose must is true
}error_table;

extern void  error_table_init      (error_table* errtab);
extern error error_table_add       (error_table* errtab, char* errtag, bool must);
extern bool  error_table_match_full(error_table* errtab, char* errtag);
extern bool  error_table_match_wdcd(error_table* errtab, char* errtag);
extern bool  error_table_exist_must(error_table* errtab);
extern void  error_table_destroy   (error_table* errtab);

#endif
