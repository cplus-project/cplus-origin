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

#define PARSE_INIT           0x00
#define PARSE_DECL_TYPE      0x01
#define PARSE_DECL_ID        0x02
#define PARSE_DECL_INIT_EXPR 0x03

// using the parsing functions based on the start symbol
// which you are meeting.
//
// note:
//   the start symbol(such as 'if', 'for', etc.) should
//   be reserved when being passed to the parsing
//   functions.
//
// example:
//   lex_analyzer lex;
//   lex_token*   lextkn;
//   ast          astree;
//   ...
//   for (;;) {
//       // lex_parse_token
//       lextkn = lex_read_token(&lex);
//       switch (lextkn->token_type) {
//       case TOKEN_ID:
//           parse_id(&lex, &astree);
//           break;
//       case TOKEN_KEYWORD_IF:
//           parse_if(&lex, &astree);
//           break;
//       case TOKEN_KEYWORD_FOR:
//           parse_for(&lex, &astree);
//           break;
//       ...
//       }
//   }
//   ...
extern error parse_id (lex_analyzer* lex, ast* astree);
extern error parse_if (lex_analyzer* lex, ast* astree);
extern error parse_for(lex_analyzer* lex, ast* astree);

#endif
