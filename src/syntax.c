/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "syntax.h"

#define peek_next_type(lex) lex_read_token(lex)->token_type

error parse_stmt_for(lex_analyzer* lex, ast* tree) {
    error      err    = NULL;
    int8       state  = FOR_INIT;
    lex_token* lextkn = NULL;
    char*      start  = NULL;
    char*      end    = NULL;
    char*      step   = NULL;
    for (;;) {
        err = lex_parse_token(lex);
        if (err != NULL) {
            return err;
        }
        lextkn = lex_read_token(lex);
        lex_next_token(lex);

        switch (state) {
        case FOR_INIT:
            switch (lextkn->token_type) {
            case TOKEN_ID:
                // the first token is an id, so peek one next token and
                // if this token after the id is:
                //   ';' => for start; end; step; {...}
                //   '{' => for end {...}
                //   ':' => for val : array {...}
                //   ',' => for val, index : array {...}
                switch (peek_next_type(lex)) {
                case TOKEN_OP_SEMC:
                    break;
                case TOKEN_OP_LBRACE:
                    break;
                case TOKEN_OP_COLON:
                    break;
                case TOKEN_OP_COMMA:
                    break;
                }
                break;
            case TOKEN_OP_LBRACE:
                // TODO: parse the stmts in the new block...
                break;
            case TOKEN_OP_SEMC:
                break;
            }
            break;
        }
    }
}
