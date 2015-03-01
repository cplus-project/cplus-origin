/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "syntax.h"

error parse_stmt_for(lex_analyzer* lex, ast* tree) {
    error      err    = NULL;
    lex_token* lextkn = NULL;
    for (;;) {
        err = lex_parse_token(lex);
        if (err != NULL) {
            return err;
        }
        lextkn = lex_read_token(lex);
        // TODO: check the for statement error here...
        lex_next_token(lex);
    }
}
