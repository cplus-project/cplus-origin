/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     The syntax.h and syntax.c implement the syntax analyzer
 * of the C+ programming language. the output parsed by the
 * analyzer will be passed and processed by the semantic analyzer.
 **/

#ifndef CPLUS_SYNTAX_H
#define CPLUS_SYNTAX_H

#include "common.h"

typedef struct operator_stack_node {
    int16 op_token_code;
    struct operator_stack_node* next;
}operator_stack_node;

typedef struct {
    operator_stack_node* top;
}operator_stack;

static void  operator_stack_init   (operator_stack* optrstk);
static void  operator_stack_push   (operator_stack* optrstk, int16 op_token_code);
static bool  operator_stack_isempty(operator_stack* optrstk);
static int16 operator_stack_top    (operator_stack* optrstk);
static void  operator_stack_pop    (operator_stack* optrstk);
static void  operator_stack_destroy(operator_stack* optrstk);

#endif
