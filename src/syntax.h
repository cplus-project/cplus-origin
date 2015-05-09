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
#include "semantic.h"

typedef struct identobj_stack_node {
    smt_identified_obj* obj;
    struct identobj_stack_node* next;
}identobj_stack_node;

// the identobj_stack is used to save some smt_identified_obj
// temporary to assist to parse ident-related syntax.
typedef struct {
   identobj_stack_node* top;
}identobj_stack;

static void                identobj_stack_init   (identobj_stack* idstk);
static void                identobj_stack_push   (identobj_stack* idstk, smt_identified_obj* obj);
static bool                identobj_stack_isempty(identobj_stack* idstk);
static smt_identified_obj* identobj_stack_top    (identobj_stack* idstk);
static void                identobj_stack_pop    (identobj_stack* idstk);
static void                identobj_stack_destroy(identobj_stack* idstk);

typedef struct oprd_stack_node {
    smt_expr* oprd;
    struct oprd_stack_node* next;
}oprd_stack_node;

// the oprd_stack is used to save a set of smt_expr to assist
// to parse the expression.
typedef struct {
    oprd_stack_node* top;
}oprd_stack;

static void      oprd_stack_init    (oprd_stack* oprdstk);
static void      oprd_stack_push    (oprd_stack* oprdstk, smt_expr* oprdexpr);
static bool      oprd_stack_isempty (oprd_stack* oprdstk);
static smt_expr* oprd_stack_top     (oprd_stack* oprdstk);
static void      oprd_stack_pop     (oprd_stack* oprdstk);
static void      oprd_stack_destroy (oprd_stack* oprdstk);

typedef struct optr_stack_node {
    int16 op_token_code;
    struct optr_stack_node* next;
}optr_stack_node;

// the optr_stack is used to assist to parse the expression.
typedef struct {
    optr_stack_node* top;
}optr_stack;

static void  optr_stack_init   (optr_stack* optrstk);
static void  optr_stack_push   (optr_stack* optrstk, int16 op_token_code);
static bool  optr_stack_isempty(optr_stack* optrstk);
static int16 optr_stack_top    (optr_stack* optrstk);
static void  optr_stack_pop    (optr_stack* optrstk);
static void  optr_stack_destroy(optr_stack* optrstk);

#define SYNTAX_ERROR_DEPENDENCE_NEEDED -1

// the syntax_analyzer is used to parse the source code based on
// the rules of C+ programming language syntax.
typedef struct syntax_analyzer {
    file_stack    file_wait_compiled; // files wait to be compiled
    file_tree     file_have_compiled; // files have been compiled
    lex_analyzer* lex;                // the lexer now using
    lex_token*    cur_token;          // the current token parsed
}syntax_analyzer;

extern void  syntax_analyzer_init                  (syntax_analyzer* syx, char* file_name);
extern error syntax_analyzer_work                  (syntax_analyzer* syx);
extern void  syntax_analyzer_destroy               (syntax_analyzer* syx);

static error syntax_analyzer_parse_include         (syntax_analyzer* syx);
static error syntax_analyzer_parse_module          (syntax_analyzer* syx);
static error syntax_analyzer_parse_block           (syntax_analyzer* syx);
static error syntax_analyzer_parse_identobj_related(syntax_analyzer* syx);
static error syntax_analyzer_parse_expr            (syntax_analyzer* syx, smt_expr* expr);
static error syntax_analyzer_parse_idex            (syntax_analyzer* syx);
static error syntax_analyzer_parse_decl            (syntax_analyzer* syx);
static error syntax_analyzer_parse_assign          (syntax_analyzer* syx);
static error syntax_analyzer_parse_branch_if       (syntax_analyzer* syx);
static error syntax_analyzer_parse_branch_switch   (syntax_analyzer* syx);
static error syntax_analyzer_parse_loop_for        (syntax_analyzer* syx);
static error syntax_analyzer_parse_loop_while      (syntax_analyzer* syx);
static error syntax_analyzer_parse_loop_infinite   (syntax_analyzer* syx);
static error syntax_analyzer_parse_loop_foreach    (syntax_analyzer* syx);
static error syntax_analyzer_parse_func_def        (syntax_analyzer* syx);
static error syntax_analyzer_parse_func_call       (syntax_analyzer* syx);
static error syntax_analyzer_parse_return          (syntax_analyzer* syx);
static error syntax_analyzer_parse_type_decl       (syntax_analyzer* syx);
static error syntax_analyzer_parse_type_assign     (syntax_analyzer* syx);
static error syntax_analyzer_parse_type_def        (syntax_analyzer* syx);
static error syntax_analyzer_parse_new             (syntax_analyzer* syx);

#endif
