/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     This this implements a red-black tree to storage the
 * information of nameable objects.
 **/

#ifndef CPLUS_IDENT_H
#define CPLUS_IDENT_H

#include "common.h"

#define ID_UNKNOWN       0x00
#define ID_CONST         0x01
#define ID_VAR           0x02
#define ID_TYPE          0x03
#define ID_FUNC          0x04
#define ID_INCLUDE       0x05
#define ID_MODULE        0x06
#define ID_MICRO         0x07

#define ACCESS_NULL      0x00
#define ACCESS_IN        0x01
#define ACCESS_OT        0x02

#define NODE_COLOR_RED   0x00
#define NODE_COLOR_BLACK 0x01

#define NODE_CMP_LT      0x00
#define NODE_CMP_EQ      0x01
#define NODE_CMP_GT      0x02

typedef struct ident         ident;
typedef struct ident_const   ident_const;
typedef struct ident_var     ident_var;
typedef struct ident_type    ident_type;
typedef struct ident_func    ident_func;
typedef struct ident_include ident_include;
typedef struct ident_module  ident_module;
typedef struct ident_table   ident_table;

// a ident_const represents a const identifier.
typedef struct ident_const {
    ident_type* const_type;
    char*       const_value;
}ident_const;

// a ident_var represents a variable identifier.
typedef struct ident_var {
    ident_type* var_type;
    char*       var_value;
}ident_var;

// a ident_type represents a type name.
typedef struct ident_type {
    ident_table* itab;
}ident_type;

typedef struct param_list_node {
    char* param_type;
    char* param_name;
    struct param_list_node* next;
}param_list_node;

typedef struct {
    param_list_node* head;
}param_list;

// a ident_type represents a function name.
typedef struct ident_func {
    param_list passin;
    param_list retout;
}ident_func;

typedef struct ident_include {
    ident_table* itab;
}ident_include;

typedef struct ident_module {
    ident_table* itab;
}ident_module;

// an ident represent an identified object.
//
// the example about using an ident object:
//    ident_table idtable;
//    ...
//    ident* id = ident_table_search(&idtable, "foo");
//    switch (id->id_type) {
//    case ID_CONST:   // do some process to the 'id->id_info.id_const'
//    case ID_VAR:     // do some process to the 'id->id_info.id_var'
//    case ID_TYPE:    // do some process to the 'id->id_info.id_type'
//    case ID_FUNC:    // do some process to the 'id->id_info.id_func'
//    case ID_INCLUDE: // do some process to the 'id->id_info.id_include'
//    case ID_MODULE:  // do some process to the 'id->id_info.id_module'
//    case ID_MICRO: ...
//    default: ...
//    }
//    ...
typedef struct ident {
    char* id_name;
    int8  access;
    int8  id_type;
    union {
        ident_const*   id_const;
        ident_var*     id_var;
        ident_type*    id_type;
        ident_func*    id_func;
        ident_include* id_include;
        ident_module*  id_module;
    }id_info;
}ident;

extern ident* make_id_const  (char* id_name, int8 access, ident_type* constant_type, char* constant_value);
extern ident* make_id_var    (char* id_name, int8 access, ident_type* variable_type, char* variable_value);
extern ident* make_id_type   (char* id_name, int8 access);
extern ident* make_id_func   (char* id_name, int8 access);
extern ident* make_id_include(char* id_name);
extern ident* make_id_module (char* id_name);

typedef struct ident_table_node {
    ident* id;
    int8   color;
    struct ident_table_node* parent;
    struct ident_table_node* lchild;
    struct ident_table_node* rchild;
}ident_table_node;

// the ident_table is used to storage a set of information
// about nameable objects in C+ language.
typedef struct ident_table {
    ident_table_node* root;
}ident_table;

extern void   ident_table_init   (ident_table* itab);
extern error  ident_table_add    (ident_table* itab, ident* id);
extern ident* ident_table_search (ident_table* itab, char*  id_name);
extern void   ident_table_destroy(ident_table* itab);

#endif
