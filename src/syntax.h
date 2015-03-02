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

#include "common.h"
#include "lex.h"
#include "ast.h"

#define FOR_INIT 0x00

// note:
//   the start symbol(such as 'if', 'for', etc.) should
//   be eliminated before being passed to the parsing
//   functions.
extern error parse_stmt_decl  (lex_analyzer* lex, ast* tree);
extern error parse_stmt_assign(lex_analyzer* lex, ast* tree);
extern error parse_stmt_if    (lex_analyzer* lex, ast* tree);
extern error parse_stmt_switch(lex_analyzer* lex, ast* tree);
extern error parse_stmt_for   (lex_analyzer* lex, ast* tree);
extern error parse_stmt_func  (lex_analyzer* lex, ast* tree);
extern error parse_stmt_return(lex_analyzer* lex, ast* tree);
extern error parse_stmt_error (lex_analyzer* lex, ast* tree);
extern error parse_stmt_deal  (lex_analyzer* lex, ast* tree);
extern error parse_stmt_type  (lex_analyzer* lex, ast* tree);

#endif
