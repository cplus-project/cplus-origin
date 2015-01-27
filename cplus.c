#include <string.h>
#include "common.h"
#include "dynamicarr.h"
#include "lex.h"

int main() {
    dynamicarr_char darr;
    error err = dynamicarr_char_init(&darr, 0);
    if (err != NULL) {
        printf("%s\r\n", err);
        return 0;
    }
    int i = 0;
    for (i = 0; i < 100; i++) {
        dynamicarr_char_appendc(&darr, 'a');
    }
    dynamicarr_char_debug(&darr);
    dynamicarr_char_destroy(&darr);

    /*char* srcfile = "/home/jikai/c_projects/temp/srcfile";
    lex_analyzer lex;
    lex_token    token;
    error        err;
    lex_init(&lex);
    err = lex_open_srcfile(&lex, srcfile);
    if (err != NULL) {
        printf("%s\r\n", err);
        return;
    }
    err = lex_parse_token(&lex, &token);
    if (err != NULL) {
        printf("%s\r\n", err);
        return;
    }
    lex_close_srcfile(&lex);*/
    return 0;
}

