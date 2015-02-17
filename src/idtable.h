/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     The idtable.h and idtable.c implement a kind of
 * table to storage the infomation about tokens defined
 * in the same scope. You can insert, update or search
 * these tokens when necessary.
 **/

#ifndef CPLUS_IDTABLE_H
#define CPLUS_IDTABLE_H

#include "common.h"

#define ID_UNKNOWN 000
#define ID_CONST   001
#define ID_VAR     002
#define ID_MOD     003

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

// the id table is a hash table.
typedef struct{
    idtable_node* ids_head[16];
}idtable;

extern void  idtable_init   (idtable* idt);
extern error idtable_insert (idtable* idt, id_info  id);
extern error idtable_update (idtable* idt, id_info  new_info);
extern error idtable_search (idtable* idt, id_info* ret);
extern void  idtable_destroy(idtable* idt);
extern void  idtable_debug  (idtable* idt);

#endif
