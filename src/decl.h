/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     The decl.h and decl.c implement a kind of list
 * to save the ids' declarations.
 **/

#ifndef CPLUS_DECL_H
#define CPLUS_DECL_H

#include "common.h"

typedef struct {
    char* decl_type;
    char* decl_name;
    char* decl_assign;
}declare;

typedef struct decl_list_node {
    declare decl;
    struct decl_list_node* next;
}decl_list_node;

// the decl_list is used to save the declares. you should
// travel the list from the head pointer by yourself.
typedef struct {
    decl_list_node* head;
    decl_list_node* tail;
}decl_list;

extern void decl_list_init   (decl_list* declist);
extern void decl_list_add    (decl_list* declist, declare decl);
extern void decl_list_destroy(decl_list* declist);
extern void decl_list_debug  (decl_list* declist);

#endif
