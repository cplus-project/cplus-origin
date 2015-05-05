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
#include "lexical.h"
#include "fileset.h"

typedef struct operator_stack_node {
    int16 op_token_code;
    struct operator_stack_node* next;
}operator_stack_node;

// the operator_stack is used to assist to parse the expression.
typedef struct {
    operator_stack_node* top;
}operator_stack;

static void  operator_stack_init   (operator_stack* optrstk);
static void  operator_stack_push   (operator_stack* optrstk, int16 op_token_code);
static bool  operator_stack_isempty(operator_stack* optrstk);
static int16 operator_stack_top    (operator_stack* optrstk);
static void  operator_stack_pop    (operator_stack* optrstk);
static void  operator_stack_destroy(operator_stack* optrstk);

// the syntax_analyzer is used to parse the source code based on
// the rules of C+ programming language syntax.
typedef struct syntax_analyzer {
    file_stack    file_wait_compiled; // files wait to be compiled
    file_tree     file_have_compiled; // files have been compiled
    lex_analyzer* lex;                // the lexer using now
}syntax_analyzer;

extern void  syntax_analyzer_init   (syntax_analyzer* syx, char* file_name);
extern error syntax_analyzer_work   (syntax_analyzer* syx);
extern void  syntax_analyzer_destroy(syntax_analyzer* syx);

#endif
