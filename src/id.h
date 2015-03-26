/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     The id_h and id.c implement a red black tree
 * to save the informations of all ids parsed from the
 * source file.
 **/

#ifndef CPLUS_IDTABLE_H
#define CPLUS_IDTABLE_H

#include "common.h"

#define ID_UNKNOWN       0x00
#define ID_CONST         0x01
#define ID_VAR           0x02
#define ID_MOD           0x03

#define NODE_COLOR_RED   0x00
#define NODE_COLOR_BLACK 0x01

#define NODE_CMP_LT      0x00
#define NODE_CMP_EQ      0x01
#define NODE_CMP_GT      0x02

// this struct is used to save the necessary information
// of the identifiers which parsed in lexical analyzing
// stage.
typedef struct {
    char* id_name;
    int8  id_type;
    char* id_datatype; // only meaningful to ID_CONST and ID_VAR
    char* id_value;    // only meaningful to ID_CONST and ID_VAR
}id;

typedef struct id_table_node{
    id   idinfo;
    int8 color; // NODE_COLOR_RED or NODE_COLOR_BLACK
    struct id_table_node* parent;
    struct id_table_node* lchild;
    struct id_table_node* rchild;
}id_table_node;

// the id table saves a set of struct 'id'. because
// it is implemented with the rbtree data-struct, the
// search performance will be always not bad for most
// situations.
typedef struct{
    id_table_node* root;
}id_table;

extern void  id_table_init   (id_table* idtab);
extern error id_table_add    (id_table* idtab, id idinfo);
extern error id_table_update (id_table* idtab, id idinfo);
extern id*   id_table_search (id_table* idtab, char* id_name);
extern void  id_table_destroy(id_table* idtab);

#endif
