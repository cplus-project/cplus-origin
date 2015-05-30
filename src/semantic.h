/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     The semantic.h and semantic.c implement the semantic
 * analyzer of the C+ programming language. The analyzer will
 * process the output from the syntax analyzer and produce
 * the final code.
 **/

#ifndef CPLUS_SEMANTIC_H
#define CPLUS_SEMANTIC_H

#include "common.h"
#include "scope.h"

#define SMT_NULL            0x00
#define SMT_INCLUDE         0x01
#define SMT_MODULE          0x02
#define SMT_CONST_LITERAL   0x03
#define SMT_IDENT           0x04
#define SMT_EXPR            0x05
#define SMT_EXPR_LIST       0x06
#define SMT_EXPR_UNARY      0x07
#define SMT_EXPR_BINARY     0x08
#define SMT_INDEX           0x09
#define SMT_DECL            0x0A
#define SMT_ASSIGN          0x0B
#define SMT_IF              0x0C
#define SMT_EF              0x0D
#define SMT_EF_LIST         0x0E
#define SMT_ELSE            0x0F
#define SMT_SWITCH          0x10
#define SMT_SWITCH_CASE     0x11
#define SMT_SWITCH_DEFAULT  0x12
#define SMT_LOOP_FOR        0x13
#define SMT_LOOP_WHILE      0x14
#define SMT_LOOP_INFINITE   0x15
#define SMT_LOOP_FOREACE    0x16
#define SMT_FUNC_DEF        0x17
#define SMT_FUNC_CALL       0x18
#define SMT_RETURN          0x19
#define SMT_TYPE_DECL       0x1A
#define SMT_TYPE_DEF        0x1B
#define SMT_NEW             0x1C
#define SMT_ERROR           0x1D
#define SMT_DEAL_SINGLE     0x1E
#define SMT_DEAL_MULTI      0x1F

typedef char*                     smt_ident;
typedef struct smt_const_literal  smt_const_literal;
typedef struct smt_include        smt_include;
typedef struct smt_module         smt_module;
typedef struct smt_expr           smt_expr;
typedef struct smt_expr_list      smt_expr_list;
typedef struct smt_expr_unary     smt_expr_unary;
typedef struct smt_expr_binary    smt_expr_binary;
typedef struct smt_index          smt_index;
typedef struct smt_decl           smt_decl;
typedef struct smt_assign         smt_assign;
typedef struct smt_if             smt_if;
typedef struct smt_ef             smt_ef;
typedef struct smt_ef_list        smt_ef_list;
typedef struct smt_else           smt_else;
typedef struct smt_switch         smt_switch;
typedef struct smt_switch_case    smt_switch_case;
typedef struct smt_switch_default smt_switch_default;
typedef struct smt_loop_for       smt_loop_for;
typedef struct smt_loop_while     smt_loop_while;
typedef struct smt_loop_infinite  smt_loop_infinite;
typedef struct smt_loop_foreach   smt_loop_foreach;
typedef struct smt_func_def       smt_func_def;
typedef struct smt_func_call      smt_func_call;
typedef struct smt_return         smt_return;
typedef struct smt_type_decl      smt_type_decl;
typedef struct smt_type_def       smt_type_def;
typedef struct smt_new            smt_new;
typedef struct smt_error          smt_error;
typedef struct smt_deal_single    smt_deal_single;
typedef struct smt_deal_multi     smt_deal_multi;

// represent the include statement:
// include <stdio.h>
// include "string.h"
// include <myfile.cplus>
typedef struct smt_include {
    char* file_name;
    struct smt_include* next;
}smt_include;

// represent the module statement:
// module fmt
// module os/user
// module net/http
typedef struct smt_module {
    char* mod_name;
    struct smt_module* next;
}smt_module;

// represent the constant literal.
typedef struct smt_const_literal {
    // lit_type:
    //    TOKEN_CONST_INTEGER or
    //    TOKEN_CONST_FLOAT   or
    //    TOKEN_CONST_CHAR    or
    //    TOKEN_CONST_STRING
    int16 const_lit_type;
    // lit_value:
    //    12, 5.21, "hello world", 'a'...
    char* const_lit_value;
}smt_const_literal;

// represent an expression.
//
// example of the struct about the expression:
//                expr
//                 |
//            expr_binary
//         (oprd1,optr,oprd2)
//           /            \
//     expr_binary       expr_unary
// (oprd1,optr,oprd2)   (optr,oprd2)
//    |          |              |
//   expr       expr           expr
//    |          |              |
//  ident  const_integer     func_call
typedef struct smt_expr {
    int8 expr_type;
    union {
        smt_ident           expr_ident;
        smt_const_literal*  expr_const_literal;
        smt_index*          expr_index;
        smt_func_def*       expr_func_def;
        smt_func_call*      expr_func_call;
        smt_type_def*       expr_type_def;
        smt_new*            expr_new;
        smt_expr_unary*     expr_unary;
        smt_expr_binary*    expr_binary;
    }expr;
}smt_expr;

typedef struct smt_expr_list_node {
    smt_expr expr;
    struct smt_expr_list_node* next;
}smt_expr_list_node;

// represent a set of expressions separated by ','
typedef struct smt_expr_list {
    smt_expr_list_node* first;
}smt_expr_list;

typedef struct smt_expr_unary {
    int16     op_token_code;
    smt_expr* oprd;
}smt_expr_unary;

typedef struct smt_expr_binary {
    int16     op_token_code;
    smt_expr* oprd1;
    smt_expr* oprd2;
}smt_expr_binary;

// represent the operation about extracting data from the container
// (e.g. array, slice).
typedef struct smt_index {
    smt_ident index_container;
    smt_expr  index_idxexpr;
}smt_index;

// represent the declaration statement.
typedef struct smt_decl {
    smt_expr* decl_type;
    smt_ident decl_name;
    smt_expr* decl_init;
}smt_decl;

// represent the assignment statement.
typedef struct smt_assign {
    smt_expr expr_lhs;
    smt_expr expr_rhs;
}smt_assign;

// represent the if statement.
typedef struct smt_if {
    smt_expr if_cond;
}smt_if;

// represent the ef(else if) statement.
typedef struct smt_ef {
    smt_expr ef_cond;
}smt_ef;

typedef struct smt_ef_list_node {
    smt_ef _ef;
    struct smt_ef_list_node* next;
}smt_ef_list_node;

// represent a set of ef semantic elements.
typedef struct smt_ef_list {
    smt_ef_list_node* first;
}smt_ef_list;

// represent the else statement.
typedef struct smt_else {} smt_else;

// represent the switch statement.
typedef struct smt_switch {
    smt_expr option;
}smt_switch;

// represent the case statement in the switch statement.
typedef struct smt_switch_case {
    smt_expr option_value;
}smt_switch_case;

// represent the default statement in the switch statement.
typedef struct smt_switch_default {} smt_switch_default;

// represent the loop statement with for-style:
// for init; cond; step {
//     ...
// }
typedef struct smt_loop_for {
    int8 loop_for_init_type;
    union {
        smt_decl*   loop_for_init_decl;
        smt_assign* loop_for_init_assign;
    }loop_for_init;
    
    smt_expr loop_for_cond;
    smt_expr loop_for_step;
}smt_loop_for;

// represent the loop statement with while-style:
// for cond {
//     ...
// } 
typedef struct smt_loop_while {
    smt_expr* loop_while_cond;
}smt_loop_while;

// represent the loop statement with infinite-style:
// for {
//     ...
// }
typedef struct smt_loop_infinite {
}smt_loop_infinite;

// represent the loop statement with foreach-style:
// for data, index : container {
//     ...
// }
typedef struct smt_loop_foreach {
    smt_expr loop_foreach_data;
    smt_expr loop_foreach_index;
    smt_expr loop_foreach_container;
}smt_loop_foreach;

typedef struct formal_param {
    smt_expr  param_type;
    smt_ident param_ident;
    struct formal_param* next;
}formal_param;

// represent the function definition:
// func func_name(params_passed) (params_return) {
//     ...
// }
typedef struct smt_func_def {
    smt_ident     func_name;
    formal_param* params_passin;
    formal_param* params_retout;
}smt_func_def;

// represent the function call:
// def  -> func foo(int x, string str) (bool ret) {}
// call -> bool ret = foo(1, "John")
typedef struct smt_func_call {
    smt_ident     func_name;
    smt_expr_list params_passin;
}smt_func_call;

// represent the return statement.
typedef struct smt_return {
    smt_expr return_val;
}smt_return;

// represent the type declaration statement.
typedef struct smt_type_decl {
    smt_ident type_name;
    smt_expr* type_bind;
}smt_type_decl;

typedef struct member_decl {
    int8      access;
    smt_expr  member_type;
    smt_ident member_name;
    struct member_decl* next;
}member_decl;

// represent the type definition:
// type {
//     in string  name
//     in int32   age
//     ot float32 score
//     ...
// }
typedef struct smt_type_def {
    member_decl* members;
}smt_type_def;

// represent the new statement:
// new int [100]
// new Person("Peter", 15, "2015-05-05") [1]
typedef struct smt_new {
    smt_expr       new_type_name;
    smt_expr_list* new_init_params; // using the default initialization if NULL
    smt_expr*      new_capacity;    // not an array if NULL
}smt_new;

typedef struct {
    scope* cur_scp; // current scope
}smt_analyzer;

extern void  smt_analyzer_init               (smt_analyzer* smt);
extern void  smt_analyzer_scope_open         (smt_analyzer* smt);
extern void  smt_analyzer_scope_close        (smt_analyzer* smt);
extern error smt_analyzer_parse_ident        (smt_analyzer* smt);
extern error smt_analyzer_parse_expr         (smt_analyzer* smt, smt_expr* expr);
extern error smt_analyzer_parse_decl         (smt_analyzer* smt, smt_decl* decl);
extern error smt_analyzer_parse_assign       (smt_analyzer* smt, smt_expr* expr_lhs, smt_expr* expr_rhs);
extern error smt_analyzer_parse_assigns      (smt_analyzer* smt, smt_expr_list* exprs_lhs, smt_expr_list* exprs_rhs);
extern error smt_analyzer_parse_branch_if    (smt_analyzer* smt);
extern error smt_analyzer_parse_loop_for     (smt_analyzer* smt);
extern error smt_analyzer_parse_loop_while   (smt_analyzer* smt);
extern error smt_analyzer_parse_loop_infinite(smt_analyzer* smt);
extern error smt_analyzer_parse_loop_foreach (smt_analyzer* smt);
extern error smt_analyzer_parse_new          (smt_analyzer* smt, smt_new* _new);
extern void  smt_analyzer_destroy            (smt_analyzer* smt);

#endif
