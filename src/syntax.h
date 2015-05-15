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
#include "close_counter.h"
#include "lexical.h"
#include "fileset.h"
#include "semantic.h"
#include "expression.h"

#define SYNTAX_ERROR_DEPENDENCE_NEEDED -1

// the syntax_analyzer is used to parse the source code based on
// the rules of C+ programming language syntax.
typedef struct syntax_analyzer {
    file_stack    file_wait_compiled; // files wait to be compiled
    file_tree     file_have_compiled; // files have been compiled
    lex_analyzer* lex;                // the lexer now using
    lex_token*    cur_token;          // the current token parsed
    close_counter clsctr;             // guarantee that all brackets are closed correctly
}syntax_analyzer;

extern void  syntax_analyzer_init               (syntax_analyzer* syx, char* file_name);
extern error syntax_analyzer_work               (syntax_analyzer* syx);
extern void  syntax_analyzer_destroy            (syntax_analyzer* syx);

static error syntax_analyzer_parse_include      (syntax_analyzer* syx);
static error syntax_analyzer_parse_module       (syntax_analyzer* syx);
static error syntax_analyzer_parse_block        (syntax_analyzer* syx);
static error syntax_analyzer_parse_expr         (syntax_analyzer* syx, smt_expr* expr, bool lhs);
static error syntax_analyzer_parse_expr_list    (syntax_analyzer* syx, smt_expr_list* exprlst);
static error syntax_analyzer_parse_idex         (syntax_analyzer* syx);
static error syntax_analyzer_parse_decl         (syntax_analyzer* syx, smt_identified_obj* decl_type, smt_ident decl_name);
static error syntax_analyzer_parse_assign       (syntax_analyzer* syx, smt_identified_obj* assign_obj);
static error syntax_analyzer_parse_branch_if    (syntax_analyzer* syx);
static error syntax_analyzer_parse_branch_switch(syntax_analyzer* syx);
static error syntax_analyzer_parse_loop_for     (syntax_analyzer* syx);
static error syntax_analyzer_parse_loop_while   (syntax_analyzer* syx);
static error syntax_analyzer_parse_loop_infinite(syntax_analyzer* syx);
static error syntax_analyzer_parse_loop_foreach (syntax_analyzer* syx);
static error syntax_analyzer_parse_func_def     (syntax_analyzer* syx);
static error syntax_analyzer_parse_func_call    (syntax_analyzer* syx, smt_func_call* call);
static error syntax_analyzer_parse_return       (syntax_analyzer* syx);
static error syntax_analyzer_parse_type_decl    (syntax_analyzer* syx);
static error syntax_analyzer_parse_type_assign  (syntax_analyzer* syx);
static error syntax_analyzer_parse_type_def     (syntax_analyzer* syx);
static error syntax_analyzer_parse_new          (syntax_analyzer* syx);

#endif
