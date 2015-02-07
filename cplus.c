#include <stdio.h>
#include <string.h>
#include "common.h"
#include "dynamicarr.h"
#include "lex.h"
#include "convert.h"
#include "scope.h"

int main() {
    scope s1;
    scope s2;
    scope s3;
    scope_init(&s1, NULL);
    scope_init(&s2, &s1);
    scope_init(&s3, &s2);
    scope* s;
    int count = 0;
    for (s = &s3; s != NULL; s = s->outer) {
        count++;
    }
    printf("%d\r\n", count);
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
