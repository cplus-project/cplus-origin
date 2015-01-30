#include <string.h>
#include "common.h"
#include "dynamicarr.h"
#include "lex.h"

int main() {
    lex_token lextkn;
    lex_token_init(&lextkn, 10);
    lex_token_append(&lextkn, "hello", 5);
    lex_token_appendc(&lextkn, '!');
    lex_token_clear(&lextkn);
    lex_token_destroy(&lextkn);
    return 0;
}
