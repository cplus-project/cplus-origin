#include <stdio.h>
#include <string.h>
#include "common.h"
#include "dynamicarr.h"
#include "lex.h"
#include "convert.h"

int main() {
    lex_analyzer lex;
    lex_init(&lex);
    lex_open_srcfile(&lex, "lex_test");

    lex_token lextkn;
    lex_token_init(&lextkn, 255);

    error err = NULL;
    for (;;) {
        err = lex_read_token(&lex, &lextkn);
        if (err != NULL) {
            break;
        }
        lex_token_debug(&lextkn);
        lex_next_token(&lex);
    }

    lex_token_destroy(&lextkn);
    lex_close_srcfile(&lex);
    return 0;
}
