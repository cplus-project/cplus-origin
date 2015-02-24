#include <stdio.h>
#include <string.h>
#include "common.h"
#include "scope.h"

int main() {
    return 0;
}

/*
int main() {
    int parse_turn = 0;
    lex_analyzer lex;
    error err;
    lex_token* token;
    err = lex_init(&lex);
    if (err != NULL) {
        printf("%s\r\n", err);
        return 0;
    }
    lex_open_srcfile(&lex, "lex_test");
    for (;;) {
        printf("turn--%04d#   ", ++parse_turn);
        err = lex_parse_token(&lex);
        if (err != NULL) {
            debug(err);
            break;
        }
        token = lex_read_token(&lex);
        lex_token_debug(token);
        lex_next_token(&lex);
    }
    lex_destroy(&lex);
    return 0;
}
*/
