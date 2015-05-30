/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     The expression.h and expression.c provide some
 * structs and functions to assist to parse expression.
 **/

#ifndef CPLUS_EXPRESSION_H
#define CPLUS_EXPRESSION_H

#include "common.h"
#include "lexical.h"
#include "semantic.h"

// the priority with a smaller number has the higher precedence.
#define OP_PRIORITY_NULL -1
#define OP_PRIORITY_0     9
#define OP_PRIORITY_1     8
#define OP_PRIORITY_2     7
#define OP_PRIORITY_3     6
#define OP_PRIORITY_4     5
#define OP_PRIORITY_5     4
#define OP_PRIORITY_6     3
#define OP_PRIORITY_7     2
#define OP_PRIORITY_8     1
#define OP_PRIORITY_9     0
extern int8 get_op_priority(int16 op_token_code);

#define OP_TYPE_LUNARY   EXTRA_INFO_OP_LUNARY
#define OP_TYPE_RUNARY   EXTRA_INFO_OP_RUNARY
#define OP_TYPE_BINARY   EXTRA_INFO_OP_BINARY
#define OP_TYPE_EXPR_END EXTRA_INFO_EXPR_END
typedef struct optr {
    int16 op_token_code; // TOKEN_OP_XXX
    int8  op_type;       // OP_TYPE_LUNARY or OP_TYPE_RUNARY or OP_TYPE_BINARY
    int8  op_priority;   // OP_PRIORITY_0 to OP_PRIORITY_9
}optr;

typedef struct optr_stack_node {
    optr op;
    struct optr_stack_node* next;
}optr_stack_node;

// the optr_stack is used to save a set of operators' token code
// to assist to parse the expression.
typedef struct {
    optr_stack_node* top;
}optr_stack;

extern void  optr_stack_init   (optr_stack* optrstk);
extern void  optr_stack_push   (optr_stack* optrstk, optr op);
extern bool  optr_stack_isempty(optr_stack* optrstk);
extern optr* optr_stack_top    (optr_stack* optrstk);
extern void  optr_stack_pop    (optr_stack* optrstk);
extern void  optr_stack_destroy(optr_stack* optrstk);

typedef struct oprd_stack_node {
    smt_expr* oprd;
    struct oprd_stack_node* next;
}oprd_stack_node;

// the oprd_stack is used to save a set of smt_expr to assist
// to parse the expression.
typedef struct {
    oprd_stack_node* top;
}oprd_stack;

extern void      oprd_stack_init      (oprd_stack* oprdstk);
extern void      oprd_stack_push      (oprd_stack* oprdstk, smt_expr* oprdexpr);
extern bool      oprd_stack_isempty   (oprd_stack* oprdstk);
extern smt_expr* oprd_stack_top       (oprd_stack* oprdstk);
extern void      oprd_stack_pop       (oprd_stack* oprdstk);
extern error     oprd_stack_calcu_once(oprd_stack* oprdstk, optr op);
extern error     oprd_stack_calcu     (oprd_stack* oprdstk, optr_stack* optrstk);
extern void      oprd_stack_destroy   (oprd_stack* oprdstk);

#endif
