#include <stdio.h>
#include <string.h>
#include "common.h"
#include "dynamicarr.h"
#include "lex.h"
#include "convert.h"
#include "scope.h"
#include "idtable.h"
#include "utf.h"

int main() {
    unicode_char rune = 0x4E25;
    utf8_char utf_val = utf8_encode(rune);
    printf("%X\r\n", utf_val);
    printf("%X\r\n", utf8_decode(utf_val));
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
