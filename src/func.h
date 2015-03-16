/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     The func.h and func.c implement a red black tree
 * to save all functions' definition and let programmers
 * to search them quickly.
 **/

#ifndef CPLUS_FUNC_TABLE_H
#define CPLUS_FUNC_TABLE_H

#include "common.h"

#define FUNC_ACCESS_IN   0x00
#define FUNC_ACCESS_OUT  0x01

#define NODE_COLOR_RED   0x00
#define NODE_COLOR_BLACK 0x01

#define NODE_CMP_LT      0x00
#define NODE_CMP_EQ      0x01
#define NODE_CMP_GT      0x02

typedef struct param_list_node {
    char* type;
    struct param_list_node* next;
}param_list_node;

// you need travel the param_list by yourself from the
// head node of the list.
typedef struct {
    param_list_node* head;
    param_list_node* tail;
}param_list;

extern void param_list_init   (param_list* paralst);
extern void param_list_add    (param_list* paralst, char* type);
extern void param_list_destroy(param_list* paralst);

typedef struct {
    int8       func_access;
    char*      func_name;
    int64      func_namelen;
    param_list func_params;
    param_list func_retval;
}func;

extern void func_init   (func* fn);
extern void func_destroy(func* fn);

typedef struct func_table_node {
    func funcinfo;
    int8 color; // NODE_COLOR_RED or NODE_COLOR_BLACK
    struct func_table_node* parent;
    struct func_table_node* lchild;
    struct func_table_node* rchild;
}func_table_node;

// the function table saves a set of struct 'func'.
// because it is implemented with the rbtree data-struct,
// the search performance will be always not bad for most
// situations.
typedef struct {
    func_table_node* root;
}func_table;

extern void  func_table_init   (func_table* functab);
extern error func_table_add    (func_table* functab, func  funcinfo);
extern error func_table_search (func_table* functab, func* search);
extern void  func_table_destroy(func_table* functab);

#endif
