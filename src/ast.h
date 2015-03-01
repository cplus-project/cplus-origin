/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     The ast.h and ast.c implement the abstract
 * syntax tree of the c+ language.
 **/

#ifndef CPLUS_AST_H
#define CPLIS_AST_H

#include <string.h>
#include "common.h"
#include "idtable.h"

#define STMT_UNKNOWN 0x000
#define STMT_DECL    0x001
#define STMT_ASSIGN  0x002
#define STMT_EXPR    0x003
#define STMT_IF      0X004
#define STMT_FOR     0x005
#define STMT_ERROR   0X006
#define STMT_DEAL    0x007
#define STMT_FUNC    0x008
#define STMT_RETURN  0x009
#define STMT_TYPE    0x00A
#define STMT_INCLUDE 0x00B
#define STMT_MODULE  0x00C

typedef struct ast_node ast_node;

// a block represents a set of statements in
// a couple of braces('{' and '}').
typedef struct {
    ast_node* stmtslist;
}stmts_block;

typedef struct include_list_node {
    char* file;
    struct include_list_node* next;
}include_list_node;

// include_list is used to save all files included
// temporary.
typedef struct include_list {
    include_list_node* head;
    include_list_node* cur;
}include_list;

extern void include_list_init   (include_list* icldlist);
extern void include_list_add    (include_list* icldlist, char* file);
extern bool include_list_exist  (include_list* icldlist, char* file);
extern void include_list_destroy(include_list* icldlist);
extern void include_list_debug  (include_list* icldlist);

typedef struct {
    ast_node* module_names;
}stmt_module;

typedef struct {
    ast_node* decl_typename;
    ast_node* decl_varname;
}stmt_decl;

typedef struct {
    ast_node* assign_left;
    ast_node* assign_right;
}stmt_assign;

typedef struct {
    ast_node* expr_operator;
    ast_node* expr_operand1;
    ast_node* expr_operand2;
}stmt_expr;

typedef struct {
    ast_node* if_expr;
    ast_node* if_stmt;
    ast_node* if_elseif;
    ast_node* if_else;
}stmt_if;

typedef struct {
    ast_node* for_assign;
    ast_node* for_end;
    ast_node* for_step;
    ast_node* for_stmt;
}stmt_for;

typedef struct {
    ast_node* error_tag;
    bool      must_deal;
}stmt_error;

typedef struct {
    ast_node* error_tags;
    ast_node* error_handle_stmts;
}stmt_deal;

typedef struct {
    ast_node* func_name;
    ast_node* func_params_decls;
    ast_node* func_retval_decls;
    ast_node* func_stmt;
}stmt_func;

typedef struct {
    ast_node* return_val;
}stmt_return;

typedef struct {
    ast_node* type_name;
    ast_node* type_body;
}stmt_type;


// the whole abstract syntax tree is composed of the struct
// ast_node, so you can travel the whole tree with only one
// ast_node pointer.
// the syntax_type and syntax_info are the most import fields
// of the ast_node. the syntax_info pointer should point to
// one of the stmt_xxxx struct according to the value of
// syntax_type.
typedef struct ast_node {
    uint32 line_count;  // record the line number of the syntax unit
    uint16 line_pos;    // record the position of the syntax unit in the line
    uint8  syntax_type; // one of the micros define prefixed with 'STMT_'
    void*  syntax_info; // point to the struct which storages the information of
                        // the syntax unit. it will be NULL if the syntax_type is
                        // STMT_UNKNOWN.
    struct ast_node* child_list_head;
}ast_node;

typedef struct {
    include_list include_files; // all file included in one source file
    idtable      modules;       // all modules imported in one source file
    // TODO: other elements...
}ast;

#endif
