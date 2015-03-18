/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "syntax.h"

error parse_id(lex_analyzer* lex, ast* astree) {
    error      err    = NULL;
    lex_token* lextkn = NULL;
    int8       state  = PARSE_INIT;
    for (;;) {
        err = lex_parse_token(lex);
        if (err != NULL) {
            return err;
        }
        lextkn = lex_read_token(lex);
        lex_next_token(lex);

        switch (state) {
        case PARSE_INIT:
            err = lex_parse_token(lex);
            if (err != NULL) {
                return err;
            }
            // peek the next token's type after the current id,
            // if it is:
            // (1) id  =>  declaration
            // (2) =   =>  assignment
            // (3) .   =>  reference the member or method
            // (4) (   =>  call function
            // (5) [   =>  reference the array
            // (6) ++  =>  increase
            // (7) --  =>  decrease
            switch (lex_read_token(lex)->token_type) {
            case TOKEN_ID:
                break;
            case TOKEN_OP_ASSIGN:
                break;
            case TOKEN_OP_SPOT:
                break;
            case TOKEN_OP_LPARENTHESE:
                break;
            case TOKEN_OP_LBRACKET:
                break;
            case TOKEN_OP_INC:
                break;
            case TOKEN_OP_DEC:
                break;
            default:
                return new_error("err: unknown context.");
            }
        }
    }
}

error parse_if(lex_analyzer* lex, ast* astree) {
}

error parse_for(lex_analyzer* lex, ast* astree) {
}
