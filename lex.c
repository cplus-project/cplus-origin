/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "lex.h"

// ----- methods of lex_token -----

error lex_token_init(lex_token* lextkn, uint64 capacity) {
    dynamicarr_char_init(&lextkn->token, capacity);
    lextkn->token_len  = 0;
    lextkn->token_type = TOKEN_UNKNOWN;
}

void lex_token_append(lex_token* lextkn, char* str, uint64 len) {
    dynamicarr_char_append(&lextkn->token, str, len);
    lextkn->token_len += len;
}

void lex_token_appendc(lex_token* lextkn, char ch) {
    dynamicarr_char_appendc(&lextkn->token, ch);
    lextkn->token_len++;
}

void lex_token_clear(lex_token* lextkn) {
    dynamicarr_char_clear(&lextkn->token);
    lextkn->token_len  = 0;
    lextkn->token_type = TOKEN_UNKNOWN;
}

void lex_token_destroy(lex_token* lextkn) {
    dynamicarr_char_destroy(&lextkn->token);
}

// ----- methods of lex_analyzer -----

void lex_init(lex_analyzer* lex) {
    lex->srcfile        = NULL;
    lex->line           = 1;
    lex->buff_end_index = 0;
    lex->i              = 0;
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

// when lex->i equals to lex->buff_end_index, it means that the source codes
// now in the lex->buffer are all processed completely. and now the lexical
// analyzer can read next LEX_BUFF_SIZE bytes source codes from the source
// file.
error lex_read_file(lex_analyzer* lex) {
    int read_len = fread(lex->buffer, 1, LEX_BUFF_SIZE, lex->srcfile);
    // process the end-of-file.
    if (feof(lex->srcfile) != 0 && read_len == 0) {
        return new_error("err: parse over.");
    }
    // process the read errors.
    if (ferror(lex->srcfile) != 0 || read_len < 0) {
        return new_error("err: some error occur when parsing.");
    }
    lex->buff_end_index = read_len;
    lex->i = 0;
}

error lex_parse_token(lex_analyzer* lex, lex_token* lextkn) {
    char ch = '0';
    lex_token_clear(lextkn);

    // skip the blank characters. all blank characters make no sense in c+.
    for (;;) {
        if (lex->i == lex->buff_end_index) {
            error err = lex_read_file(lex);
            if (err != NULL) {
                return err;
            }
        }
        ch = lex->buffer[lex->i];
        if (ch == ' ') {
            lex->i++;
        }
        else if (ch == '\r' || ch == '\n') {
            lex->i++;
            // counting the line numbers to make some preparations for
            // debuging and error/warning reporting.
            lex->line++;
        } else {
            break;
        }
    }

    // parsing identifers
    if (('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') || ch == '_') {
        lex_token_appendc(lextkn, ch);
        lex->i++;
        for (;;) {
            if (lex->i == lex->buff_end_index) {
                error err = lex_read_file(lex);
                if (err != NULL) {
                    return err;
                }
            }
            ch = lex->buffer[lex->i];
            if (('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') || ('0' <= ch && ch <= '9') || ch == '_') {
                lex_token_appendc(lextkn, ch);
                lex->i++;
            }
            else {
                char* token_content = dynamicarr_char_getstr(lextkn->token);
                if ('b' <= token_content[0] && token_content[0] <= 'f') {
                    // for func
                    if (lextkn->token_len == 3 && strcmp(token_content, "for") == 0) {
                        lextkn->token_type = TOKEN_KEYWORD_FOR;
                        return NULL;
                    }
                    else if (lextkn->token_len == 4 && strcmp(token_content, "func") == 0) {
                        lextkn->token_type = TOKEN_KEYWORD_FUNC;
                        return NULL;
                    }
                    // ef else
                    if (lextkn->token_len == 2 && strcmp(token_content, "ef") == 0) {
                        lextkn->token_type = TOKEN_KEYWORD_EF;
                        return NULL;
                    }
                    else if (lextkn->token_len == 4 && strcmp(token_content, "else")) {
                        lextkn->token_type = TOKEN_KEYWORD_ELSE;
                        return NULL;
                    }
                    // break
                    if (lextkn->token_len == 5 && strcmp(token_content, "break") == 0) {
                        lextkn->token_type = TOKEN_KEYWORD_BREAK;
                        return NULL;
                    }
                    // deal
                    if (lextkn->token_len == 4 && strcmp(token_content, "deal") == 0) {
                        lextkn->token_type = TOKEN_KEYWORD_DEAL;
                        return NULL;
                    }
                    // case continue
                    if (lextkn->token_len == 4 && strcmp(token_content, "case") == 0) {
                        lextkn->token_type = TOKEN_KEYWORD_CASE;
                        return NULL;
                    }
                    else if (lextkn->token_len == 8 && strcmp(token_content, "continue") == 0) {
                        lextkn->token_type = TOKEN_KEYWORD_CONTINUE;
                        return NULL;
                    }
                }
                switch (token_content[0]) {
                case 'i': // if in
                    if (lextkn->token_len == 2 && strcmp(token_content, "if") == 0) {
                        lextkn->token_type = TOKEN_KEYWORD_IF;
                        return NULL;
                    }
                    else if (lextkn->token_len == 2 && strcmp(token_content, "in") == 0) {
                        lextkn->token_type = TOKEN_KEYWORD_IN;
                        return NULL;
                    }
                case 't': // type
                    if (lextkn->token_len == 4 && strcmp(token_content, "type") == 0) {
                        lextkn->token_type = TOKEN_KEYWORD_TYPE;
                        return NULL;
                    }
                case 'r': // return
                    if (lextkn->token_len == 6 && strcmp(token_content, "return") == 0) {
                        lextkn->token_type = TOKEN_KEYWORD_RETURN;
                        return NULL;
                    }
                case 'o': // ot
                    if (lextkn->token_len == 2 && strcmp(token_content, "ot") == 0) {
                        lextkn->token_type = TOKEN_KEYWORD_OT;
                        return NULL;
                    }
                }
                // variable or constant names
                lextkn->token_type = TOKEN_ID;
                return NULL;
            }
        }
    }
}

error lex_peek_token(lex_analyzer* lex, lex_token* lextkn) {

}
