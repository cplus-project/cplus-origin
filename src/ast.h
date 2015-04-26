/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     This file implements the abstract syntax tree of
 * the C+ language.
 **/

#ifndef CPLUS_AST_H
#define CPLUS_AST_H

#include "common.h"

#define AST_NODE_NULL            NULL
#define AST_NODE                 0x01
#define AST_NODE_INCLUDE         0x02
#define AST_NODE_MODULE          0x03
#define AST_NODE_BLOCK           0x03
#define AST_NODE_CONST_INTEGER   0x04
#define AST_NODE_CONST_FLOAT     0x05
#define AST_NODE_CONST_CHAR      0x06
#define AST_NODE_CONST_STRING    0x07
#define AST_NODE_ID              0x08
#define AST_NODE_ID_SET          0x09
#define AST_NODE_INDEX           0x10
#define AST_NODE_EXPR            0x11
#define AST_NODE_EXPR_UNARY      0x12
#define AST_NODE_EXPR_BINARY     0x13
#define AST_NODE_DECL            0x14
#define AST_NODE_ASSIGN          0x15
#define AST_NODE_FUNC_DEF        0x16
#define AST_NODE_FUNC_CALL       0x17
#define AST_NODE_RETURN          0x18
#define AST_NODE_TYPE_DECL       0x19
#define AST_NODE_TYPE_ASSIGN     0x20
#define AST_NODE_TYPE_DEF        0x21
#define AST_NODE_NEW             0x22
#define AST_NODE_ERROR           0x23
#define AST_NODE_DEAL_SINGLE     0x24
#define AST_NODE_DEAL_MULTI      0x25
#define AST_NODE_IF              0x26
#define AST_NODE_EF              0x27
#define AST_NODE_ELSE            0x28
#define AST_NODE_BRANCH_IF       0x29
#define AST_NODE_SWITCH_CASE     0x30
#define AST_NODE_SWITCH_DEFAULT  0x31
#define AST_NODE_BRANCH_SWITCH   0x32
#define AST_NDOE_LOOP_FOR        0x33
#define AST_NODE_LOOP_WHILE      0x34
#define AST_NODE_LOOP_INFINITE   0x35
#define AST_NODE_LOOP_FOREACH    0x36

typedef struct ast_node                ast_node;
typedef struct ast_node_block          ast_node_block;
typedef struct ast_node_include        ast_node_include;
typedef struct ast_node_module         ast_node_module;
typedef struct ast_node_const_integer  ast_node_const_integer;
typedef struct ast_node_const_float    ast_node_const_float;
typedef struct ast_node_const_char     ast_node_const_char;
typedef struct ast_node_const_string   ast_node_const_string;
typedef struct ast_node_id             ast_node_id;
typedef struct ast_node_id_set         ast_node_id_set;
typedef struct ast_node_index          ast_node_index;
typedef struct ast_node_expr           ast_node_expr;
typedef struct ast_node_expr_unary     ast_node_expr_unary;
typedef struct ast_node_expr_binary    ast_node_expr_binary;
typedef struct ast_node_decl           ast_node_decl;
typedef struct ast_node_assign         ast_node_assign;
typedef struct ast_node_func_def       ast_node_func_def;
typedef struct ast_node_func_call      ast_node_func_call;
typedef struct ast_node_return         ast_node_return;
typedef struct ast_node_type_decl      ast_node_type_decl;
typedef struct ast_node_type_assign    ast_node_type_assign;
typedef struct ast_node_type_def       ast_node_type_def;
typedef struct ast_node_new            ast_node_new;
typedef struct ast_node_error          ast_node_error;
typedef struct ast_node_deal_single    ast_node_single;
typedef struct ast_node_deal_multi     ast_node_deal_multi;
typedef struct ast_node_if             ast_node_if;
typedef struct ast_node_ef             ast_node_ef;
typedef struct ast_node_else           ast_node_else;
typedef struct ast_node_branch_if      ast_node_branch_if;
typedef struct ast_node_switch_case    ast_node_switch_case;
typedef struct ast_node_switch_default ast_node_switch_default;
typedef struct ast_node_branch_switch  ast_node_branch_switch;
typedef struct ast_node_loop_for       ast_node_loop_for;
typedef struct ast_node_loop_while     ast_node_loop_while;
typedef struct ast_node_loop_infinite  ast_node_loop_infinite;
typedef struct ast_node_loop_foreach   ast_node_loop_foreach;

extern ast_node*         new_ast_node        (int8 node_type, void* node);
extern ast_node_include* new_ast_node_include(char* file_name);
extern ast_node_module*  new_ast_node_module (char* module_name);
extern ast_node_block*   new_ast_node_block  ();

// the ast_node can represent any other node.
typedef struct ast_node {
    int8  node_type;
    void* node;
    ast_node* next;
}ast_node;

// the ast_node_include represent a file included in the project
// with the 'include' keyword.
//
// example:
//    include <stdio.h>
//    include <string.h>
//    include "myio.h"
typedef struct ast_node_include {
    char* file_name;
}ast_node_include;

// the ast_node_module represent a module imported in the project
// with the 'module' keyword.
//
// example:
//    module fmt
//    module net/http
typedef struct ast_node_module {
    char* module_name;
}ast_node_module;

typedef struct ast_node_block_node {
    ast_node* node;
    struct ast_node_block_node* next;
}ast_node_block_node;

// the ast_node_block represent a set of statements between a pair
// of braces('{' and '}').
typedef struct ast_node_block {
    ast_node_block_node* head;
    ast_node_block_node* tail;
}ast_node_block;

extern void ast_node_block_addstmt(ast_node_block* block, ast_node* node);

typedef struct {

}abstract_syntax_tree;

#endif
