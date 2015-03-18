/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     This file is the main function file. it organizes
 * and composes all other functions needed by the compiler.
 **/

#include <string.h>
#include "common.h"
#include "lex.h"
#include "ast.h"

int main(int argc, char* argv[]) {
    error err = NULL;

    if (argc < 2) {
        // TODO: display the program usage manual
        return 0;
    }

    // command:
    //   build    build the specific C+ project
    //   run      build and run the specific C+ project
    //   test     test the specific C+ file or project
    //   help     display the manual
    char* command = argv[1];
    if (strcmp(command, "build") == 0) {
        char* filename = argv[2];
        if (filename == NULL || strlen(filename) <= 0) {
            fprintf(stderr, "error: not specify the file to build.\r\n");
            fprintf(stderr, "usage: cplus build [filename]\r\n");
            return 0;
        }

        lex_analyzer lex;
        lex_token*   lextkn;
        ast          astree;
        // TODO: initialized the astree !!!
        // TODO: destroy the astree !!!

        err = lex_init(&lex);
        if (err != NULL) {
            fprintf(stderr, "%s\r\n", err);
            return 0;
        }
        err = lex_open_srcfile(&lex, filename);
        if (err != NULL) {
            fprintf(stderr, "%s\r\n", err);
            return 0;
        }
        for (;;) {
            err = lex_parse_token(&lex);
            if (err != NULL) {
                break;
            }
            // the syntax parser read the every token
            // here and use them to build the ast.
            lextkn = lex_read_token(&lex);
            switch (lextkn->token_type) {
            case TOKEN_ID:
                parse_id(&lex, &astree);
                break;
            case TOKEN_KEYWORD_IF:
                parse_if(&lex, &astree);
                break;
            case TOKEN_KEYWORD_FOR:
                parse_for(&lex, &astree);
                break;
            }
        }

        lex_close_srcfile(&lex);
        lex_destroy(&lex);
    }

    return 0;
}

