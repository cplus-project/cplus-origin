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

typedef struct property_list_node {
    char* property_type;
    char* property_name;
    struct property_list_node* next;
}property_list_node;

// this list is used to save the type's properties' defination.
// you should travel the list from the head by yourself.
typedef struct {
    property_list_node* head;
    property_list_node* tail;
}property_list;

extern void property_list_init   (property_list* pptlist);
extern void property_list_add    (property_list* pptlist, char* property_type, char* property_name);
extern void property_list_destroy(property_list* pptlist);

typedef struct {
    int8          type_access;     // the accessibility of the type, TYPE_ACCESS_IN or TYPE_ACCESS_OUT
    char*         type_name;       // the name of the type, the first letter should be upper case
    property_list type_properties; // the properties(or called members) of the type
    func_table    type_methods;    // the methods(or called behaviour) of the type
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
extern type* type_table_search (type_table* typetab, char* type_name);
extern void  type_table_destroy(type_table* typetab);

#endif
