#include <stdio.h>
#include <string.h>
#include "common.h"
#include "dynamicarr.h"
#include "lex.h"
#include "convert.h"
#include "scope.h"
#include "idtable.h"

int main() {
    idtable idt;
    idtable_init(&idt);

    id_info id[5];
    id[0].id_name     = "req";
    id[0].id_len      = 3;
    id[0].id_type     = ID_VAR;
    id[0].id_datatype = "Request";
    id[0].id_value    = "nil";

    id[1].id_name     = "buffer";
    id[1].id_len      = 6;
    id[1].id_type     = ID_VAR;
    id[1].id_datatype = "Buffer";
    id[1].id_value    = "nil";

    id[2].id_name     = "i";
    id[2].id_len      = 1;
    id[2].id_type     = ID_VAR;
    id[2].id_datatype = "int32";
    id[2].id_value    = "0";

    id[3].id_name     = "j";
    id[3].id_len      = 1;
    id[3].id_type     = ID_VAR;
    id[3].id_datatype = "uint8";
    id[3].id_value    = "1";

    id[4].id_name     = "jsondata";
    id[4].id_len      = 8;
    id[4].id_type     = ID_VAR;
    id[4].id_datatype = "JsonData";
    id[4].id_value    = "{name: 'Cplus', hver: 1, lver: 1}";

    int i;
    for (i = 0; i < 5; i++) {
        idtable_insert(&idt, id[i]);
    }

    idtable_debug(&idt);
    idtable_destroy(&idt);
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
