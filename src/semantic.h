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

#define SMT_ELEM_NULL          0x00
#define SMT_ELEM               0x00
#define SMT_ELEM_CONST_INTEGER 0x01
#define SMT_ELEM_CONST_FLOAT   0x02
#define SMT_ELEM_CONST_CHAR    0x03
#define SMT_ELEM_CONST_STRING  0x04
#define SMT_IDENT              0x05
#define SMT_IDENTIFIED_OBJ     0x06
#define SMT_EXPR               0x11
#define SMT_EXPR_UNARY         0x12
#define SMT_EXPR_BINARY        0x13
#define SMT_INDEX              0x08
#define SMT_DECL               0x09
#define SMT_ASSIGN             0x10
#define SMT_IF                 0x13
#define SMT_EF                 0x00
#define SMT_ELSE               0x00
#define SMT_SWITCH             0x00
#define SMT_SWITCH_CASE        0x00
#define SMT_SWITCH_DEFAULT     0x00
#define SMT_LOOP_FOR           0x00
#define SMT_LOOP_WHILE         0x00
#define SMT_LOOP_INFINITE      0x00
#define SMT_LOOP_FOREACE       0x00
#define SMT_FUNC_DEF           0x14
#define SMT_FUNC_CALL          0x15
#define SMT_RETURN             0x16
#define SMT_TYPE_DECL          0x17
#define SMT_TYPE_ASSIGN        0x18
#define SMT_TYPE_DEF           0x19
#define SMT_NEW                0x20
#define SMT_ERROR              0x21
#define SMT_DEAL_SINGLE        0x22
#define SMT_DEAL_MULTI         0x23

typedef char* smt_const_integer;
typedef char* smt_const_float;
typedef char  smt_const_char;
typedef char* smt_const_string;
typedef char* smt_ident;

typedef struct smt_identified_obj smt_identified_obj;
typedef struct smt_expr           smt_expr;
typedef struct smt_expr_unary     smt_expr_unary;
typedef struct smt_expr_binary    smt_expr_binary;
typedef struct smt_index          smt_index;
typedef struct smt_decl           smt_decl;
typedef struct smt_assign         smt_assign;
typedef struct smt_if             smt_if;
typedef struct smt_ef             smt_ef;
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
typedef struct smt_type_assign    smt_type_assign;
typedef struct smt_type_def       smt_type_def;
typedef struct smt_new            smt_new;
typedef struct smt_error          smt_error;
typedef struct smt_deal_single    smt_deal_single;
typedef struct smt_deal_multi     smt_deal_multi;

// represent one or a set of identified objects(e.g. identifier,
// index, function).
typedef struct smt_identified_obj {
    int8 obj_type;
    union {
        smt_ident      obj_ident;
        smt_index*     obj_index;
        smt_func_call* obj_func_call;
    }obj;
    
    struct smt_identified_obj* denoted;
}smt_identified_obj;

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
        smt_const_integer   expr_const_integer;
        smt_const_float     expr_const_float;
        smt_const_char      expr_const_char;
        smt_const_string    expr_const_string;
        smt_ident           expr_ident;
        smt_identified_obj* expr_identified_obj;
        smt_index*          expr_index;
        smt_func_call*      expr_func_call;
        smt_expr_unary*     expr_unary;
        smt_expr_binary*    expr_binary;
    }expr;
}smt_expr;

typedef struct smt_expr_unary {
    int16    op_token_code;
    smt_expr oprd;
}smt_expr_unary;

typedef struct smt_expr_binary {
    int16    op_token_code;
    smt_expr oprd1;
    smt_expr oprd2;
}smt_expr_binary;

// represent the operation about extracting data from the container
// (e.g. array, slice).
typedef struct smt_index {
    smt_identified_obj index_obj;
    smt_expr           index_idx;
}smt_index;

// represent the declaration statement.
typedef struct smt_decl {
    smt_identified_obj decl_type;
    smt_ident          decl_name;
}smt_decl;

// represent the assignment statement.
typedef struct smt_assign {
    smt_identified_obj assign_obj;
    smt_expr           assign_expr;
}smt_assign;

typedef struct smt_if {
    smt_expr if_cond;
}smt_if;

typedef struct smt_ef {
    smt_expr ef_cond;
}smt_ef;

typedef struct smt_else {
}smt_else;

typedef struct smt_switch {
}smt_switch;

typedef struct smt_switch_case {
    // TODO:
}smt_switch_case;

typedef struct smt_switch_default {
    // TODO:
}smt_switch_default;

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
    smt_expr loop_while_cond;
}smt_loop_while;

// represent the loop statement with infinite-style:
// for {
//     ...
// }
typedef struct smt_loop_infinite {
}smt_loop_infinite;

// represent the loop statement with foreach-style:
// (1) for data : container {  | (2) for data, index : container {
//         ...                 |         ...
//     }                       |     }
typedef struct smt_loop_foreach {
    smt_identified_obj loop_foreach_data;
    smt_identified_obj loop_foreach_index;
    smt_identified_obj loop_foreach_container;
}smt_loop_foreach;

#endif
