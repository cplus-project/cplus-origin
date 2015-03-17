/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     The ast.h and ast.c implement the abstract
 * syntax tree of the C+ language.
 **/

#ifndef CPLUS_AST_H
#define CPLIS_AST_H

#include <string.h>
#include "common.h"
#include "id.h"
#include "scope.h"

#define STMT_UNKNOWN
#define STMT_BLOCK
#define STMT_ASSIGN
#define STMT_LOOP_FOR
#define STMT_LOOP_WHILE
#define STMT_LOOP_INFINITE
#define STMT_LOOP_FOREACH

typedef struct ast_node ast_node;

typedef struct include_list_node {
    char* file;
    struct include_list_node* next;
}include_list_node;

// include_list is used to save all files included
// temporary.
typedef struct {
    include_list_node* head;
    include_list_node* tail;
}include_list;

extern void include_list_init   (include_list* icldlist);
extern void include_list_add    (include_list* icldlist, char* file);
extern bool include_list_exist  (include_list* icldlist, char* file);
extern void include_list_destroy(include_list* icldlist);
extern void include_list_debug  (include_list* icldlist);

typedef struct stmt_block_entry {
    ast_node* astnode;
    struct stmt_block_entry* next;
}stmt_block_entry;

// a block represents a set of statements in
// a couple of braces('{' and '}').
typedef struct {
    stmt_block_entry* fst;
    stmt_block_entry* lst;
}stmt_block;

extern void stmt_block_init   (stmt_block* block);
extern void stmt_block_add    (stmt_block* block, ast_node* astnode);
extern void stmt_block_destroy(stmt_block* block);

typedef struct {
    char* oprd;
    int16 optr; // the operator's token type code
}expr_unary;

typedef struct {
    char* oprd1;
    char* oprd2;
    int16 optr; // the operator's token type code
}expr_binary;

typedef struct {
    id_info id;
    // expr
}stmt_assign;

typedef struct {
    stmt_assign assign;
    // end
    // step
    scope      scp;
    stmt_block stmts;
}stmt_loop_for;

// the whole abstract syntax tree is composed of the struct
// ast_node, so you can travel the whole tree with only one
// ast_node pointer.
//
// note:
//   the syntax_type and syntax_info are the most import fields
//   of the ast_node. the syntax_info pointer should point to
//   one of the stmt_xxxx struct according to the value of
//   syntax_type.
typedef struct ast_node {
    int32 line_count;  // record the line number of the syntax unit
    int16 line_pos;    // record the position of the syntax unit in the line
    int8  syntax_type; // one of the micros define prefixed with 'STMT_'
    // point to the struct which storages the elements of one syntax
    // unit. it will be NULL if the syntax_type is STMT_UNKNOWN.
    union {
        stmt_block* block;
    }syntax_form;
}ast_node;

typedef struct {
    include_list include_files; // all file included in one source file
    idtable      modules;       // all modules imported in one source file
    // TODO: other elements...
}ast;

#endif
