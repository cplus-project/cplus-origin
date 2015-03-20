/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     The test for token_buffer in src/syntax.h and
 * src/syntax.c
 **/

#include "../syntax.h"

int main() {
    int       i;
    error     err = NULL;
    lex_token tkn[5];
    char*     str[5] = {
    "if",
    "for",
    "buffer",
    "hello world",
    "0b10010011"
    };
    for (i = 0; i < 5; i++) {
        lex_token_init(&tkn[i], 64);
        lex_token_append(&tkn[i], str[i], strlen(str[i]));
    }
    tkn[0].token_type = TOKEN_KEYWORD_IF;
    tkn[1].token_type = TOKEN_KEYWORD_FOR;
    tkn[2].token_type = TOKEN_ID;
    tkn[3].token_type = TOKEN_CONST_STRING;
    tkn[4].token_type = TOKEN_CONST_NUMBER;

    token_buffer tknbuff;
    token_buffer_init(&tknbuff);
    for (i = 0; i < 5; i++) {
        token_buffer_push(&tknbuff, tkn[i]);
    }
    for (;;) {
        if (token_buffer_isempty(&tknbuff) == true) {
            break;
        }
        printf("%s\r\n", lex_token_getstr(&tknbuff.top->tkninfo));
        err = token_buffer_pop(&tknbuff);
        if (err != NULL) {
            debug(err);
        }
    }
    token_buffer_destroy(&tknbuff);

    for (i = 0; i < 5; i++) {
        lex_token_destroy(&tkn[i]);
    }

    debug("\r\n\r\nrun over~");
    return 0;
}
