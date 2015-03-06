/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     The type.h and type.c implement a red black tree
 * to save the informations of types defined in a C+
 * project and let the programmers search them very quickly.
 **/

#ifndef CPLUS_TYPE_TABLE_H
#define CPLUS_TYPE_TABLE_H

#include "common.h"
#include "id.h"
#include "func.h"

#define TYPE_ACCESS_IN   0x00
#define TYPE_ACCESS_OUT  0x01

#define NODE_COLOR_RED   0x00
#define NODE_COLOR_BLACK 0x01

#define NODE_CMP_LT      0x00
#define NODE_CMP_EQ      0x01
#define NODE_CMP_GT      0x02

typedef struct {
    int8       type_access;     // the accessibility of the type, TYPE_ACCESS_IN or TYPE_ACCESS_OUT
    char*      type_name;       // the name of the type, the first letter should be upper case
    uint64     type_namelen;    // the length of the type name
    decl_list  type_properties; // the properties(or called members) of the type
    func_table type_methods;    // the methods(or called behaviour) of the type
}type;

extern void type_init   (type* typ);
extern void type_destroy(type* typ);

typedef struct type_table_node {
    type typeinfo;
    int8 color; // NODE_COLOR_RED or NODE_COLOR_BLACK
    struct type_table_node* parent;
    struct type_table_node* lchild;
    struct type_table_node* rchild;
}type_table_node;

// the type table saves a set of struct 'type'. because
// it is implemented with the rbtree data-struct, the
// search performance will be always not bad for most
// situations.
typedef struct {
    type_table_node* root;
}type_table;

extern void  type_table_init   (type_table* typetab);
extern error type_table_add    (type_table* typetab, type  typeinfo);
extern error type_table_search (type_table* typetab, type* search);
extern void  type_table_destroy(type_table* typetab);

typedef struct type_table_iterator_node {
    type_table_node* log;
    struct type_table_iterator_node* next;
}type_table_iterator_node;

// type_table_iterator is a stack which used to travel
// the type_table. the traveling is often in postorder
// because this order has no need for extra visited-flag.
typedef struct {
    type_table_iterator_node* top;
}type_table_iterator;

extern void             type_table_iterator_init    (type_table_iterator* it);
extern void             type_table_iterator_push    (type_table_iterator* it, type_table_node* log);
extern type_table_node* type_table_iterator_top     (type_table_iterator* it);
extern error            type_table_iterator_pop     (type_table_iterator* it);
extern void             type_table_iterator_destroy (type_table_iterator* it);
extern void             type_table_iterator_debug   (type_table_iterator* it);

#endif
