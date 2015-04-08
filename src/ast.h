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
#include "type.h"

#define STMT_UNKNOWN       0x00
#define STMT_BLOCK         0x00
#define STMT_CONST_INTEGER 0x00
#define STMT_CONST_FLOAT   0x00
#define STMT_CONST_CHAR    0x00
#define STMT_CONST_STRING  0x00
#define STMT_OPERATOR      0x00
#define STMT_ID            0x00
#define STMT_FUNC_DEF      0x00
#define STMT_FUNC_CALL     0x00
#define STMT_EXPR_UNARY    0x00
#define STMT_EXPR_BINARY   0x00
#define STMT_ASSIGN        0x00
#define STMT_IF            0x00
#define STMT_EF            0x00
#define STMT_ELSE          0x00
#define STMT_LOOP_FOR      0x00
#define STMT_LOOP_WHILE    0x00
#define STMT_LOOP_INFINITE 0x00
#define STMT_LOOP_FOREACH  0x00
#define STMT_ERROR         0x00
#define STMT_DEAL_SINGLE   0x00
#define STMT_DEAL_MULTI    0x00

typedef struct ast_node ast_node;

typedef struct include_list_node {
    char* file;
    struct include_list_node* next;
}include_list_node;

// include_list is used to save all file names included
// temporary.
typedef struct {
    include_list_node* head;
    include_list_node* tail;
}include_list;

extern void include_list_init   (include_list* icldlist);
extern void include_list_add    (include_list* icldlist, char* file);
extern bool include_list_exist  (include_list* icldlist, char* file);
extern void include_list_destroy(include_list* icldlist);

typedef struct module_list_node {
    char* module;
    struct module_list_node* next;
}module_list_node;

// module_list is used to save all module names included
// temporary.
typedef struct {
    module_list_node* head;
    module_list_node* tail;
}module_list;

extern void module_list_init   (module_list* modlist);
extern void module_list_add    (module_list* modlist, char* module);
extern bool module_list_exist  (module_list* modlost, char* module);
extern void module_list_destroy(module_list* modlist);

// a block represents a set of statements in
// a couple of braces('{' and '}').
typedef struct {
    scope     scp;
    ast_node* fst;
}stmt_block;

extern void stmt_block_init   (stmt_block* block, stmt_block* outer);
extern void stmt_block_destroy(stmt_block* block);

typedef struct {
    char* value;
}stmt_const_integer;

typedef struct {
    char* value;
}stmt_const_float;

typedef struct {
    char value;
}stmt_const_char;

typedef struct {
    char* value;
}stmt_const_string;

typedef struct {
    int16 op_type;
}stmt_operator;

typedef struct {
    id idinfo;
}stmt_id;

typedef struct {
    char*     func_name;
    decl_list params;
    decl_list retval;
    ast_node* implement;
}stmt_func_def;

typedef struct {
    decl_list retval;
    char* func_name;
    decl_list params;
}stmt_func_call;

typedef struct {
    int16     optr_code; // the operator's token code
    ast_node* oprd;
}stmt_expr_unary;

typedef struct {
    int16     optr_code; // the operator's token code
    ast_node* oprd1;
    ast_node* oprd2;
}stmt_expr_binary;

typedef struct {
    id        idinfo;
    ast_node* expr;
}stmt_assign;

typedef struct {
    ast_node* expr;
    ast_node* block;
}stmt_if;

typedef struct {
    ast_node* expr;
    ast_node* block;
}stmt_ef;

typedef struct {
    ast_node* block;
}stmt_else;

typedef struct {
    ast_node* init;
    ast_node* cond;
    ast_node* post;
    ast_node* block;
}stmt_loop_for;

typedef struct {
    ast_node* cond;
    ast_node* block;
}stmt_loop_while;

typedef struct {
    ast_node* block;
}stmt_loop_infinite;

typedef struct {
    ast_node* data;
    ast_node* idx;
    ast_node* arr;
    ast_node* block;
}stmt_loop_foreach;

typedef struct {
    char* tag;
    bool  is_must;
}stmt_error;

typedef struct {
    char*     tag;
    ast_node* block;
}stmt_deal_single;

typedef struct {
    // TODO: implement a list like "tag list"...
}stmt_deal_multi;

// the whole abstract syntax tree is composed of the struct
// ast_node, so you can travel the whole tree with one
// ast_node pointer.
//
// note:
//   the syntax_type and syntax_info are the most import fields
//   of the ast_node. ** the syntax_info pointer should point to
//   one of the stmt_xxxx struct according to the value of
//   syntax_type. **
typedef struct ast_node {
    int32 line_count;  // record the line number of the syntax unit
    int8  syntax_type; // one of the micros define prefixed with 'STMT_'
    // point to the struct which storages the elements of one syntax
    // unit. it will not be used if the syntax_type is STMT_UNKNOWN.
    union {
        stmt_block*         syntax_block;
        stmt_const_integer* syntax_const_integer;
        stmt_const_float*   syntax_const_float;
        stmt_const_char*    syntax_const_char;
        stmt_const_string*  syntax_const_string;
        stmt_operator*      syntax_operator;
        stmt_id*            syntax_id;
        stmt_expr_unary*    syntax_expr_unary;
        stmt_expr_binary*   syntax_expr_binary;
    }syntax_entity;

    struct ast_node* next;
}ast_node;

typedef struct ast_node_stack_node {
    ast_node node;
    struct ast_node_stack_node* next;
}ast_node_stack_node;

// the ast_node_stack is used to store ast nodes in a stack for
// some specific purposes. for example, this data-struct can be
// used to parse expression and condition.
typedef struct {
    ast_node_stack_node* top;
}ast_node_stack;

extern void      ast_node_stack_init   (ast_node_stack* stk);
extern void      ast_node_stack_push   (ast_node_stack* stk, ast_node node);
extern error     ast_node_stack_pop    (ast_node_stack* stk);
extern ast_node* ast_node_stack_top    (ast_node_stack* stk);
extern void      ast_node_stack_destroy(ast_node_stack* stk);

typedef struct {
    include_list include_files; // all file included in one source file
    module_list  modules;       // all modules imported in one source file
    ast_node*    fst;           // always be the stmt_block (global block)
    ast_node*    cur;
    stmt_block*  cur_block;     // always point to the now parsing block
}ast;

extern void ast_init   (ast* astree);
extern void ast_destroy(ast* astree);

#endif
