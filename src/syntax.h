/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     The syntax.h and syntax.c implment the syntax
 * analyzer. the syntax analyzer is used to check the
 * syntax errors and generate the abstract syntax tree.
 **/

#ifndef CPLUS_SYNTAX_H
#define CPLUS_SYNTAX_H

#include <string.h>
#include "common.h"
#include "dynamicarr.h"
#include "lex.h"
#include "ast.h"

// the syntax_analyzer is the parser of the C+. it will check
// the syntax and generate the ast.
typedef struct {
    lex_analyzer   lex;
    lex_token*     cur_token;
    ast            astree;
    error_list     err_list;
}syntax_analyzer;

extern error syntax_analyzer_init        (syntax_analyzer* syx, char* file);
extern ast*  syntax_analyzer_generate_ast(syntax_analyzer* syx);
extern void  syntax_analyzer_destroy     (syntax_analyzer* syx);

#endif
