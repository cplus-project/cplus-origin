#include <string.h>
#include "common.h"
#include "lex.h"

int main() {
    char* srcfile = "/home/jikai/c_projects/temp/srcfile";
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
    lex_close_srcfile(&lex);
    return 0;
}

