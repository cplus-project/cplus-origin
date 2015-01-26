#include "lex.h"

void lex_init(lex_analyzer* lex) {
    lex->srcfile = NULL;
    lex->line    = 1;
    lex->i       = 0;
    memset(lex->buffer, 0, LEX_BUFF_SIZE);
}

error lex_open_srcfile(lex_analyzer* lex, char* file) {
    lex->srcfile = fopen(file, "r");
    if (lex == NULL) {
        return new_error("not found the source file.");
    }
    return NULL;
}

void lex_close_srcfile(lex_analyzer* lex) {
    if (lex->srcfile == NULL) {
        printf("err: no file opened.\n");
        exit(EXIT_FAILURE);
    }
    fclose(lex->srcfile);
}

error lex_parse_token(lex_analyzer* lex, lex_token* lextkn) {
    assert(lex->srcfile != NULL);
    for (;;) {
        // when lex->i equals to LEX_BUFF_SIZE, it means that the source codes now
        // in the lex->buffer are all processed completely. and now the lexical
        // analyzer can read next LEX_BUFF_SIZE bytes source codes from the source
        // file.
        if (lex->i == LEX_BUFF_SIZE) {
            int read_len = fread(lex->buffer, 1, LEX_BUFF_SIZE, lex->srcfile);
            if (read_len != LEX_BUFF_SIZE) {
                if (feof(lex->srcfile) != 0) {
                    return new_error("err: parse over.");
                } else if (ferror(lex->srcfile) != 0) {
                    return new_error("err: some error occur when parsing.");
                } else {
                    return new_error("err: parsing failed with unknown error.");
                }
            }
            lex->i = 0;
            continue;
        }

        // skip the blank characters. all blank characters make no sense in c+.
        char ch = lex->buffer[lex->i];
        if (ch == ' ' || ch == '\r' || ch == '\n') {
            continue;
        }

        if (('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') || ch == '_') {

        }


        lex->i++;
    }
}

error lex_peek_token(lex_analyzer* lex, lex_token* lextkn) {

}
