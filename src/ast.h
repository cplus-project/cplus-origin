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

#define AST_ELEM_NULL             -1
#define AST_ELEM_UNKNOWN        0x00
#define AST_ELEM                0x01
#define AST_ELEM_BLOCK          0x02
#define AST_ELEM_CONST_INTEGER  0x03
#define AST_ELEM_CONST_FLOAT    0x04
#define AST_ELEM_CONST_CHAR     0x05
#define AST_ELEM_CONST_STRING   0x06
#define AST_ELEM_ID             0x07
#define AST_ELEM_ARRELEM        0x08
#define AST_ELEM_DEREFER        0x09
#define AST_ELEM_DECL           0x0A
#define AST_ELEM_ASSIGN         0x0B
#define AST_ELEM_EXPR_UNARY     0x0C
#define AST_ELEM_EXPR_BINARY    0x0D
#define AST_ELEM_IF             0x0E
#define AST_ELEM_EF             0x0F
#define AST_ELEM_ELSE           0x10
#define AST_ELEM_BRANCH_IF      0x11
#define AST_ELEM_SWITCH_CASE    0x12
#define AST_ELEM_SWITCH_DEFAULT 0x13
#define AST_ELEM_BRANCH_SWITCH  0x14
#define AST_ELEM_LOOP_FOR       0x15
#define AST_ELEM_LOOP_WHILE     0x16
#define AST_ELEM_LOOP_INFINITE  0x17
#define AST_ELEM_LOOP_FOREACH   0x18
#define AST_ELEM_TYPE_DECL      0x19
#define AST_ELEM_TYPE_BIND      0x1A
#define AST_ELEM_TYPE_DEF       0x1B
#define AST_ELEM_NEW            0x1C
#define AST_ELEM_FUNC_DEF       0X1D
#define AST_ELEM_FUNC_CALL      0x1E
#define AST_ELEM_RET            0x1F
#define AST_ELEM_ERROR          0x20
#define AST_ELEM_DEAL_SINGLE    0x21
#define AST_ELEM_DEAL_MULTI     0x22

typedef struct ast_elem                ast_elem;
typedef struct ast_elem_block          ast_elem_block;
typedef struct ast_elem_const_integer  ast_elem_const_integer;
typedef struct ast_elem_const_float    ast_elem_const_float;
typedef struct ast_elem_const_char     ast_elem_const_char;
typedef struct ast_elem_const_string   ast_elem_const_string;
typedef struct ast_elem_id             ast_elem_id;
typedef struct ast_elem_arrelem        ast_elem_arrelem;
typedef struct ast_elem_derefer        ast_elem_derefer;
typedef struct ast_elem_decl           ast_elem_decl;
typedef struct ast_elem_assign         ast_elem_assign;
typedef struct ast_elem_expr_unary     ast_elem_expr_unary;
typedef struct ast_elem_expr_binary    ast_elem_expr_binary;
typedef struct ast_elem_if             ast_elem_if;
typedef struct ast_elem_ef             ast_elem_ef;
typedef struct ast_elem_else           ast_elem_else;
typedef struct ast_elem_branch_if      ast_elem_branch_if;
typedef struct ast_elem_switch_case    ast_elem_switch_case;
typedef struct ast_elem_switch_default ast_elem_switch_default;
typedef struct ast_elem_branch_switch  ast_elem_branch_switch;
typedef struct ast_elem_loop_for       ast_elem_loop_for;
typedef struct ast_elem_loop_while     ast_elem_loop_while;
typedef struct ast_elem_loop_infinite  ast_elem_loop_infinite;
typedef struct ast_elem_loop_foreach   ast_elem_loop_foreach;
typedef struct ast_elem_type_decl      ast_elem_type_decl;
typedef struct ast_elem_type_bind      ast_elem_type_bind;
typedef struct ast_elem_type_def       ast_elem_type_def;
typedef struct ast_elem_new            ast_elem_new;
typedef struct ast_elem_func_def       ast_elem_func_def;
typedef struct ast_elem_func_call      ast_elem_func_call;
typedef struct ast_elem_ret            ast_elem_ret;
typedef struct ast_elem_error          ast_elem_error;
typedef struct ast_elem_deal_single    ast_elem_deal_single;
typedef struct ast_elem_deal_multi     ast_elem_deal_multi;

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

typedef struct ast_elem {
    int32 line_count;
    int16 line_pos;

    // the members ast_elem_type and ast_elem_entity should work cooperatively.
    // the value of ast_elem_type is one of the micro-defines prefixed with
    // AST_ELEM_XXXX, and the real content of the ast_elem_entity corresponds
    // to with the ast_elem_type's value.
    //
    // for example:
    // (1) if the ast_elem_type is AST_ELEM_LOOP_FOR, the valid content of the
    //     ast_elem_entity is ast_elem_entity.elem_loop_for
    // (2) if the ast_elem_type is AST_ELEM_TYPE_DEF, the valid content of the
    //     ast_elem_entity is ast_elem_entity.elem_type_def
    int8 ast_elem_type;
    union {
        ast_elem_block*         elem_block;
        ast_elem_const_integer* elem_const_integer;
        ast_elem_const_float*   elem_const_float;
        ast_elem_const_char*    elem_const_char;
        ast_elem_const_string*  elem_const_string;
        ast_elem_id*            elem_id;
        ast_elem_arrelem*       elem_arrelem;
        ast_elem_derefer*       elem_derefer;
        ast_elem_decl*          elem_decl;
        ast_elem_assign*        elem_assign;
        ast_elem_if*            elem_if;
        ast_elem_branch_if*     elem_branch_if;
        ast_elem_branch_switch* elem_branch_switch;
        ast_elem_loop_for*      elem_loop_for;
        ast_elem_loop_while*    elem_loop_while;
        ast_elem_loop_infinite* elem_loop_infinite;
        ast_elem_loop_foreach*  elem_loop_foreach;
        ast_elem_type_decl*     elem_type_decl;
        ast_elem_type_bind*     elem_type_bind;
        ast_elem_type_def*      elem_type_def;
        ast_elem_new*           elem_new;
        ast_elem_func_def*      elem_func_def;
        ast_elem_func_call*     elem_func_call;
        ast_elem_ret*           elem_ret;
        ast_elem_error*         elem_error;
        ast_elem_deal_single*   elem_deal_single;
        ast_elem_deal_multi*    elem_deal_multi;
    }ast_elem_entity;
}ast_elem;

typedef struct ast_elem_block_node {
    ast_elem* elem;
    struct ast_elem_block_node* prev;
    struct ast_elem_block_node* next;
}ast_elem_block_node;

// a block represents a braced ('{' and '}') statement list.
typedef struct ast_elem_block {
    ast_elem_block_node* head;
    ast_elem_block_node* tail;
}ast_elem_block;

extern void      ast_elem_block_init        (ast_elem_block* block);
extern void      ast_elem_block_add         (ast_elem_block* block, ast_elem* elem);
extern bool      ast_elem_block_isempty     (ast_elem_block* block);
extern ast_elem* ast_elem_block_get_back    (ast_elem_block* block);
extern int8      ast_elem_block_get_backtype(ast_elem_block* block);
extern error     ast_elem_block_remove_back (ast_elem_block* block);
extern error     ast_elem_block_replace_back(ast_elem_block* block, ast_elem* elem);
extern void      ast_elem_block_destroy     (ast_elem_block* block);

// represent the literal integer
typedef struct ast_elem_const_integer {
    int32 line_count;
    int16 line_pos;
    char* value;
}ast_elem_const_integer;

// represent the literal float number
typedef struct ast_elem_const_float {
    int32 line_count;
    int16 line_pos;
    char* value;
}ast_elem_const_float;

// represent the literal character
typedef struct ast_elem_const_char {
    int32 line_count;
    int16 line_pos;
    char  value;
}ast_elem_const_char;

// represent the literal string
typedef struct ast_elem_const_string {
    int32 line_count;
    int16 line_pos;
    char* value;
}ast_elem_const_string;

// the id will trigger searching symbol-table operation.
// you can make a choice to select which table to do the searching operation
// firstly based on the id's format style:
// (1) XxxXxxXxx -> the id may be a typename
// (2) xxxXxxXxx -> the id may be a function name or method name
// (3) xx_xx_xxx -> the id may be a variable name or const variable name
typedef struct ast_elem_id {
    int32 line_count;
    int16 line_pos;
    char* id_name;
}ast_elem_id;

// an arrelem represents an array-element-extraction operation.
// example: arr[0], arr[i], arr[datas.getIndex()] ...
typedef struct ast_elem_arrelem {
    int32 line_count;
    int16 line_pos;

    int8 arrlabel_type;
    union {
        ast_elem_id*        arrlabel_id;
        ast_elem_arrelem*   arrlabel_arrelem;
        ast_elem_derefer*   arrlabel_derefer;
        ast_elem_func_call* arrlabel_func_call;
    }arrlabel;

    int8 arrindex_type;
    union {
        ast_elem_const_integer* arrindex_const_integer;
        ast_elem_id*            arrindex_id;
        ast_elem_arrelem*       arrindex_arrelem;
        ast_elem_derefer*       arrindex_derefer;
        ast_elem_func_call*     arrindex_func_call;
    }arrindex;
}ast_elem_arrelem;

// a derefer represents a dereference operation.
// example: person.name, student.getScore(), arr[i].getVal() ...
typedef struct ast_elem_derefer {
    int32 line_count;
    int16 line_pos;

    int8 derefer_left_type;
    union {
        ast_elem_id*        derefer_left_id;
        ast_elem_arrelem*   derefer_left_arrelem;
        ast_elem_derefer*   derefer_left_derefer;
        ast_elem_func_call* derefer_left_func_call;
        ast_elem_new*       derefer_left_new;
    }derefer_left;

    int8 derefer_right_type;
    union {
        ast_elem_id*        derefer_right_id;
        ast_elem_func_call* derefer_right_func_call;
    }derefer_right;
}ast_elem_derefer;

// a decl represents a declaration.
// example: int number = 5
typedef struct ast_elem_decl {
    int8 decl_type_type;
    union {
        ast_elem_id*      decl_type_id;
        ast_elem_arrelem* decl_type_arrelem;
        ast_elem_derefer* decl_type_derefer;
    }decl_type;

    char* decl_name;

    int8 decl_init_type;
    union {
        ast_elem_expr_unary*  decl_init_expr_unary;
        ast_elem_expr_binary* decl_init_expr_binary;
    }decl_init;
}ast_elem_decl;

// a assign represent a assignment operation.
// example: name = "JiKai", stds[12].score = 91 ...
typedef struct ast_elem_assign {
    int8 assign_left_type;
    union {
        ast_elem_id*      assign_left_id;
        ast_elem_arrelem* assign_left_arrelem;
        ast_elem_derefer* assign_left_derefer;
    }assign_left;

    int8 assign_right_type;
    union {
        ast_elem_expr_unary*  assign_right_unary;
        ast_elem_expr_binary* assign_right_binary;
    }assign_right;
}ast_elem_assign;

typedef struct ast_elem_expr_unary {
    int16 optr_token_code;

    int8 oprd_type;
    union {
        ast_elem_const_integer* oprd_const_integer;
        ast_elem_const_float*   oprd_const_float;
        ast_elem_const_char*    oprd_const_char;
        ast_elem_const_string*  oprd_const_string;
        ast_elem_id*            oprd_id;
        ast_elem_arrelem*       oprd_arrelem;
        ast_elem_derefer*       oprd_derefer;
        ast_elem_func_call*     oprd_func_call;
        ast_elem_expr_unary*    oprd_expr_unary;
        ast_elem_expr_binary*   oprd_expr_binary;
    }oprd;
}ast_elem_expr_unary;

typedef struct ast_elem_expr_binary {
    int16 optr_token_code;

    int8 loprd_type;
    union {
        ast_elem_const_integer* oprd_const_integer;
        ast_elem_const_float*   oprd_const_float;
        ast_elem_const_char*    oprd_const_char;
        ast_elem_const_string*  oprd_const_string;
        ast_elem_id*            oprd_id;
        ast_elem_arrelem*       oprd_arrelem;
        ast_elem_derefer*       oprd_derefer;
        ast_elem_func_call*     oprd_func_call;
        ast_elem_expr_unary*    oprd_expr_unary;
        ast_elem_expr_binary*   oprd_expr_binary;
    }loprd;

    int8 roprd_type;
    union {
        ast_elem_const_integer* oprd_const_integer;
        ast_elem_const_float*   oprd_const_float;
        ast_elem_const_char*    oprd_const_char;
        ast_elem_const_string*  oprd_const_string;
        ast_elem_id*            oprd_id;
        ast_elem_arrelem*       oprd_arrelem;
        ast_elem_derefer*       oprd_derefer;
        ast_elem_func_call*     oprd_func_call;
        ast_elem_expr_unary*    oprd_expr_unary;
        ast_elem_expr_binary*   oprd_expr_binary;
    }roprd;
}ast_elem_expr_binary;

typedef struct ast_elem_if {
    int8 if_cond_type;
    union {
        ast_elem_expr_unary*  if_cond_expr_unary;
        ast_elem_expr_binary* if_cond_expr_binary;
    }if_cond;

    ast_elem_block* if_block;
}ast_elem_if;

typedef struct ast_elem_ef {
    int8 ef_cond_type;
    union {
        ast_elem_expr_unary*  ef_cond_expr_unary;
        ast_elem_expr_binary* ef_cond_expr_binary;
    }ef_cond;

    ast_elem_block* ef_block;
}ast_elem_ef;

typedef struct ast_elem_else {
    ast_elem_block* else_block;
}ast_elem_else;

typedef struct ef_list_node {
    ast_elem_ef* ef;
    struct ef_list_node* next;
}ef_list_node;

// the ef_list is used to save a set of ef statement.
typedef struct {
    ef_list_node* head;
    ef_list_node* tail;
}ef_list;

extern void ef_list_init   (ef_list* eflist);
extern void ef_list_add    (ef_list* eflist, ast_elem_ef* ef);
extern void ef_list_destroy(ef_list* eflist);

// a branch_if represents the if-ef(optional)-else(optional) statement:
typedef struct ast_elem_branch_if {
    ast_elem_if*   branch_if_if;
    ef_list*       branch_if_eflist; // no ef   statement if set NULL
    ast_elem_else* branch_if_else;   // no else statement if set NULL
}ast_elem_branch_if;

typedef struct ast_elem_switch_case {
    int8 switch_case_value_type;
    union {
        ast_elem_const_integer* switch_case_value_const_integer;
        ast_elem_const_float*   switch_case_value_const_float;
        ast_elem_const_char*    switch_case_value_const_char;
        ast_elem_const_string*  switch_case_value_const_string;
        ast_elem_id*            switch_case_value_id; // the id must be a const
    }switch_case_value;

    ast_elem_block* switch_case_block;
}ast_elem_switch_case;

typedef struct ast_elem_switch_default {
    ast_elem_block* switch_default_block;
}ast_elem_switch_default;

typedef struct switch_case_list_node {
    ast_elem_switch_case* switch_case;
    struct switch_case_list_node* next;
}switch_case_list_node;

// the switch_case_list is used to save a set of case statements of
// the switch statement.
typedef struct {
    switch_case_list_node* head;
    switch_case_list_node* tail;
}switch_case_list;

extern void switch_case_list_init   (switch_case_list* caselist);
extern void switch_case_add         (switch_case_list* caselist, ast_elem_switch_case* switch_case);
extern void switch_case_list_destroy(switch_case_list* caselist);

// a brach_switch represents the switch statement:
// switch expr {
// case const_expr1: fallthrough
// case const_expr2:
// ...
// default:
// }
typedef struct ast_elem_branch_switch {
    int8 switch_expr_type;
    union {
        ast_elem_expr_unary*  switch_expr_unary;
        ast_elem_expr_binary* switch_expr_binary;
    }switch_expr;

    switch_case_list*        caselist;
    ast_elem_switch_default* defaultcase;
}ast_elem_branch_switch;

// a loop_for represents a for init; cond; step {...} style loop statment.
typedef struct ast_elem_loop_for {
    ast_elem_assign* loop_for_init;

    int8 loop_for_cond_type;
    union {
        ast_elem_expr_unary*  loop_for_cond_expr_unary;
        ast_elem_expr_binary* loop_for_cond_expr_binary;
    }loop_for_cond;

    int8 loop_for_step_type;
    union {
        ast_elem_expr_unary*  loop_for_step_expr_unary;
        ast_elem_expr_binary* loop_for_step_expr_binary;
    }loop_for_step;

    ast_elem_block* loop_for_block;
}ast_elem_loop_for;

// a loop_while represents a for cond {...} style loop statment;
typedef struct ast_elem_loop_while {
    int8 loop_while_cond_type;
    union {
        ast_elem_expr_unary*  loop_while_cond_expr_unary;
        ast_elem_expr_binary* loop_while_cond_expr_binary;
    }loop_while_cond;

    ast_elem_block* loop_while_block;
}ast_elem_loop_while;

// a loop_infinite represent a for {...} style loop statement;
typedef struct ast_elem_loop_infinite {
    ast_elem_block* loop_infinite_block;
}ast_elem_loop_infinite;

// a loop_foreach represents a for data, index(optional) : arr {...} style
// loop statement.
typedef struct ast_elem_loop_foreach {
    int8 loop_foreach_data_type;
    union {
        ast_elem_id*      loop_foreach_data_id;
        ast_elem_arrelem* loop_foreach_data_arrelem;
        ast_elem_derefer* loop_foreach_data_derefer;
        ast_elem_decl*    loop_foreach_data_decl;
    }loop_foreach_data;

    int8 loop_foreach_index_type;
    union {
        ast_elem_id*      loop_foreach_index_id;
        ast_elem_arrelem* loop_foreach_index_arrelem;
        ast_elem_derefer* loop_foreach_index_derefer;
        ast_elem_decl*    loop_foreach_index_decl;
    }loop_foreach_index;

    int8 loop_foreach_arr_type;
    union {
        ast_elem_id*        loop_foreach_arr_id;
        ast_elem_arrelem*   loop_foreach_arr_arrelem;
        ast_elem_derefer*   loop_foreach_arr_derefer;
        ast_elem_func_call* loop_foreach_arr_func_call;
    }loop_foreach_arr;

    ast_elem_block* loop_foreach_block;
}ast_elem_loop_foreach;

typedef struct {
    char* decl_type;
    char* decl_name;
    int8  access_mode;
}declare;

typedef struct decl_list_node {
    declare* decl;
    struct decl_list_node* next;
}decl_list_node;

// the decl_list is used to save a set of declarations.
// it can be used to represents the members of the custom type or
// formal parameters of the function definitions.
typedef struct decl_list {
    decl_list_node* head;
    decl_list_node* tail;
}decl_list;

extern void decl_list_init   (decl_list* decllist);
extern void decl_list_add    (decl_list* decllist, declare* decl);
extern void decl_list_destroy(decl_list* decllist);

// a type_decl represents a type label declaration statement.
// example: type Person, type LinkButton, type SystemInfo ...
typedef struct ast_elem_type_decl {
    char* type_name;
}ast_elem_type_decl;

// a type_bind represents a binding operation between one type
// label and one or a set of type definition.
// example:
// (1) type Person = type {...}
// (2) type Student = Person + type {...}
typedef struct ast_elem_type_bind {
    int8 type_bind_left_type;
    union {
        ast_elem_id*      type_bind_left_id;
        ast_elem_derefer* type_bind_left_derefer;
    }type_bind_left;
    
    int8 type_bind_right_type;
    union {
        ast_elem_id*          type_bind_right_id;
        ast_elem_expr_binary* type_bind_right_expr_binary;
        ast_elem_type_def*    type_bind_right_type_def;
    }type_bind_right;
}ast_elem_type_bind;

// a type_def represents a type definition.
// example:
// type {
//     in string id
//     in string name
//     ot int8   age
// }
typedef struct ast_elem_type_def {
    decl_list* member_list;
}ast_elem_type_def;

// a func_def represents a function definition.
// example: func funcName(Type1 para1, Type2 para2) (Type1 ret1, Type2 ret2) {...}
typedef struct ast_elem_func_def {
    char*           func_name;
    decl_list*      params_list;
    decl_list*      retval_list;
    ast_elem_block* func_def_block;
}ast_elem_func_def;

typedef struct ast_elem_stack_node {
    ast_elem* elem;
    struct ast_elem_stack_node* next;
}ast_elem_stack_node;

// the ast_elem_stack is used to store a set of ast_elem structs in a stack
// for some specific purposes. for example, this data-struct can be used to
// parse expression.
typedef struct {
    ast_elem_stack_node* top;
}ast_elem_stack;

extern void      ast_elem_stack_init   (ast_elem_stack* stk);
extern void      ast_elem_stack_push   (ast_elem_stack* stk, ast_elem* elem);
extern error     ast_elem_stack_pop    (ast_elem_stack* stk);
extern ast_elem* ast_elem_stack_top    (ast_elem_stack* stk);
extern void      ast_elem_stack_destroy(ast_elem_stack* stk);

typedef struct {
    include_list    include_files; // all file included in one source file
    module_list     modules;       // all modules imported in one source file
    ast_elem_block* global_block;
}ast;

extern void ast_init   (ast* astree);
extern void ast_destroy(ast* astree);

#endif
