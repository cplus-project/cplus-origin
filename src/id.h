/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     The id_h and id.c implement a kind of table to
 * save the infomation about tokens defined in the
 * same scope. You can insert, update or search these
 * tokens when necessary.
 *     There is also a kind of list to save the ids'
 * declarations.
 **/

#ifndef CPLUS_IDTABLE_H
#define CPLUS_IDTABLE_H

#include "common.h"

#define ID_UNKNOWN 0x00
#define ID_CONST   0x01
#define ID_VAR     0x02
#define ID_MOD     0x03

// this struct is used to save the necessary information
// of the identifiers which parsed in lexical analyzing
// stage.
typedef struct id_info{
    char*  id_name;
    uint64 id_len;
    int8   id_type;
    char*  id_datatype; // only meaningful to ID_CONST and ID_VAR
    char*  id_value;    // only meaningful to ID_CONST and ID_VAR
}id_info;

typedef struct idtable_node{
    id_info id;
    struct idtable_node* next;
}idtable_node;

// the id table is a hash table. the detail of the hash
// implementation is the idtable_hash(...) function in
// id.c
typedef struct{
    idtable_node* ids_head[16];
}idtable;

extern void  idtable_init   (idtable* idt);
extern error idtable_insert (idtable* idt, id_info  id);
extern error idtable_update (idtable* idt, id_info  new_info);
extern error idtable_search (idtable* idt, id_info* ret);
extern void  idtable_destroy(idtable* idt);
extern void  idtable_debug  (idtable* idt);

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
    decl_list_node* cur;
}decl_list;

extern void decl_list_init   (decl_list* declist);
extern void decl_list_add    (decl_list* declist, declare decl);
extern void decl_list_destroy(decl_list* declist);
extern void decl_list_debug  (decl_list* declist);

#endif
