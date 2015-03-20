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

typedef struct token_buffer_node {
    lex_token tkninfo;
    struct token_buffer_node* next;
}token_buffer_node;

// the token_buffer is used to save the tokens related
// to the current parsing context.
typedef struct {
    token_buffer_node* top;
}token_buffer;

extern void       token_buffer_init   (token_buffer* tknbuff);
extern bool       token_buffer_isempty(token_buffer* tknbuff);
extern void       token_buffer_push   (token_buffer* tknbuff, lex_token tkninfo);
extern lex_token* token_buffer_top    (token_buffer* tknbuff);
extern error      token_buffer_pop    (token_buffer* tknbuff);
extern void       token_buffer_destroy(token_buffer* tknbuff);

// the syntax_analyzer is the parser of the C+. it
// will check the syntax and generate the ast.
typedef struct {
    lex_analyzer lex;
    token_buffer tkn_buff;
    ast          astree;
}syntax_analyzer;

extern void  syntax_analyzer_init        (syntax_analyzer* syx, char* file);
extern error syntax_analyzer_generate_ast(syntax_analyzer* syx);
extern void  syntax_analyzer_destroy     (syntax_analyzer* syx);

#endif
