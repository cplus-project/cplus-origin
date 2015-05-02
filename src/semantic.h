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

#define SMT_ELEM               0x00
#define SMT_ELEM_CONST_INTEGER 0x01
#define SMT_ELEM_CONST_FLOAT   0x02
#define SMT_ELEM_CONST_CHAR    0x03
#define SMT_ELEM_CONST_STRING  0x04
#define SMT_IDENT              0x05
#define SMT_IDENTIFIED_OBJ     0x06
#define SMT_IDENTIFIED_OBJS    0x07
#define SMT_INDEX              0x08
#define SMT_DECL               0x09
#define SMT_ASSIGN             0x10
#define SMT_EXPR               0x11
#define SMT_EXPR_UNARY         0x12
#define SMT_EXPR_BINARY        0x13
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

// TODO: need a method to represent the 'block'
typedef struct smt_identified_obj  smt_identified_obj;
typedef struct smt_identified_objs smt_identified_objs;
typedef struct smt_index           smt_index;
typedef struct smt_decl            smt_decl;
typedef struct smt_assign          smt_assign;
typedef struct smt_expr            smt_expr;
typedef struct smt_expr_unary      smt_expr_unary;
typedef struct smt_expr_binary     smt_expr_binary;
typedef struct smt_func_def        smt_func_def;
typedef struct smt_func_call       smt_func_call;
typedef struct smt_return          smt_return;
typedef struct smt_type_decl       smt_type_decl;
typedef struct smt_type_assign     smt_type_assign;
typedef struct smt_type_def        smt_type_def;
typedef struct smt_new             smt_new;
typedef struct smt_error           smt_error;
typedef struct smt_deal_single     smt_deal_single;
typedef struct smt_deal_multi      smt_deal_multi;

typedef struct smt_identified_obj {
    int8 obj_type;
    union {
        smt_ident      obj_ident;
        smt_index*     obj_index;
        smt_func_call* obj_func_call;
    }obj;
}smt_identified_obj;

typedef struct smt_identified_objs {
    smt_identified_obj  obj;
    smt_identified_obj* next;
}smt_identified_objs;

typedef struct smt_index {
    smt_identified_obj* obj;
    int                 idx;
}smt_index;

#endif
