/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "lex.h"

// you should always use the lex_idx_inc(lex->i) to
// increase the buffer's current index of the lexical
// analyzer.
#define lex_next(lex_analyzer) \
lex_analyzer->i++;                                     \
if (lex_analyzer->i >= lex_analyzer->buff_end_index) { \
    error err = lex_read_file(lex_analyzer);           \
    if (err != NULL) {                                 \
        return err;                                    \
    }                                                  \
}

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

char* lex_token_getstr(lex_token* lextkn) {
    return dynamicarr_char_getstr(&lextkn->token);
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

// read the source code from source file to buffer.
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

char lex_readc(lex_analyzer* lex) {
    return lex->buffer[lex->i];
}

// used to parse scientific notation part of a number
// constant. you shoud use this as below:
//   ...
//   for (;;) {
//       char ch = lex_readc(&lex);
//       if (ch == 'e' || ch == 'E') {
//           lex_next(lex);
//           error err = lex_parse_scientific_notation(&lex, lextkn);
//           if (err != NULL) {
//               return err;
//           }
//           else {
//               lextkn->token_type = TOKEN_CONST_NUMBER;
//               return NULL;
//           }
//       }
//   }
//   ...
error lex_parse_scientific_notation(lex_analyzer* lex, lex_token* lextkn) {
    lex_token_appendc(lextkn, 'E');
    char ch = lex_readc(lex);
    if (ch == '-' || ch == '+') {
        lex_token_appendc(lextkn, ch);
        lex_next(lex);
    }
    for (;;) {
        ch = lex_readc(lex);
        if ('0' <= ch && ch <= '9') {
            lex_token_appendc(lextkn, ch);
            lex_next(lex);
        }
        else {
            lextkn->token_type = TOKEN_CONST_NUMBER;
            return NULL;
        }
    }
}

error lex_parse_token(lex_analyzer* lex, lex_token* lextkn) {
    char ch = '0';
    lex_token_clear(lextkn);
    if (lex->i == lex->buff_end_index) {
        error err = lex_read_file(lex);
        if (err != NULL) {
            return err;
        }
    }

    // skip the blank characters. all blank characters make no sense in c+.
    for (;;) {
        ch = lex_readc(lex);
        if (ch == ' ') {
            lex_next(lex);
        }
        else if (ch == '\r' || ch == '\n') {
            lex_next(lex);
            // counting the line numbers to make some preparations for
            // debuging and error/warning reporting.
            lex->line++;
        }
        else {
            break;
        }
    }

    // parsing identifers and keywords
    if (('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') || ch == '_') {
        lex_token_appendc(lextkn, ch);
        lex_next(lex);
        for (;;) {
            ch = lex_readc(lex);
            if (('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') || ('0' <= ch && ch <= '9') || ch == '_') {
                lex_token_appendc(lextkn, ch);
                lex_next(lex);
            }
            else {
                char* token_content = lex_token_getstr(lextkn);
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

    // parsing integer constants and float constants
    if ('0' <= ch && ch <= '9') {
        char first = ch;
        lex_token_appendc(lextkn, ch);
        lex_next(lex);
        // distinguish the different number formats, such as:
        // (1) ordinary format
        // (2) 0b... or 0B...
        // (3) 0o... or 0O...
        // (4) 0d... or 0D...
        // (5) 0x... or 0X...
        if (first != '0') {
            for (;;) {
                ch = lex_readc(lex);
                // integer part
                if ('0' <= ch && ch <= '9') {
                    lex_token_appendc(lextkn, ch);
                    lex_next(lex);
                }
                // floating part
                else if (ch == '.') {
                    lex_token_appendc(lextkn, ch);
                    lex_next(lex);
                    for (;;) {
                        ch = lex_readc(lex);
                        if ('0' <= ch && ch <= '9') {
                            lex_token_appendc(lextkn, ch);
                            lex_next(lex);
                        }
                        else if (ch == 'e' || ch == 'E') { // scientific notation part
                            lex_next(lex);
                            error err = lex_parse_scientific_notation(lex, lextkn);
                            if (err != NULL) {
                                return err;
                            }
                            else {
                                lextkn->token_type = TOKEN_CONST_NUMBER;
                                return NULL;
                            }
                        }
                        else {
                            lextkn->token_type = TOKEN_CONST_NUMBER;
                            return NULL;
                        }
                    }
                }
                else if (ch == 'e' || ch == 'E') { // scientific notation part
                    lex_next(lex);
                    error err = lex_parse_scientific_notation(lex, lextkn);
                    if (err != NULL) {
                        return err;
                    }
                    else {
                        lextkn->token_type = TOKEN_CONST_NUMBER;
                        return NULL;
                    }
                }
                else {
                    lextkn->token_type = TOKEN_CONST_NUMBER;
                    return NULL;
                }
            }
        }
        else {
            ch = lex_readc(lex);
            switch (ch) {
            case 'x':
            case 'X': // hex format
                lex_token_appendc(lextkn, ch);
                lex_next(lex);
                for (;;) {
                    ch = lex_readc(lex);
                    if (('0' <= ch && ch <= '9') || ('A' <= ch && ch <= 'F') || ('a' <= ch && ch <= 'f')) {
                        lex_token_appendc(lextkn, ch);
                        lex_next(lex);
                    }
                    else {
                        lextkn->token_type = TOKEN_CONST_NUMBER;
                        return NULL;
                    }
                }
                break;
            case 'b':
            case 'B': // binary format
                lex_next(lex);
                lex_token_clear(lextkn);
                for (;;) {
                    ch = lex_readc(lex);
                    if (ch == '0' || ch == '1') {
                        lex_token_appendc(lextkn, ch);
                        lex_next(lex);
                    }
                    else {
                        char* decimal = conv_itoa(conv_binary_to_decimal(lex_token_getstr(lextkn), lextkn->token_len));
                        lex_token_clear(lextkn);
                        lex_token_append(lextkn, decimal, strlen(decimal));
                        lextkn->token_type = TOKEN_CONST_NUMBER;
                        return NULL;
                    }
                }
                break;
            case 'o':
            case 'O': // octal format
                lex_next(lex);
                for (;;) {
                    ch = lex_readc(lex);
                    if ('0' <= ch && ch <= '7') {
                        lex_token_appendc(lextkn, ch);
                        lex_next(lex);
                    }
                    else {
                        lextkn->token_type = TOKEN_CONST_NUMBER;
                        return NULL;
                    }
                }
                break;
            case 'd':
            case 'D': // decimal format
                lex_next(lex);
                lex_token_clear(lextkn);
                for (;;) {
                    ch = lex_readc(lex);
                    if ('0' <= ch && ch <= '9') {
                        lex_token_appendc(lextkn, ch);
                        lex_next(lex);
                    }
                    else {
                        lextkn->token_type = TOKEN_CONST_NUMBER;
                        return NULL;
                    }
                }
                break;
            }
        }
    }
    // parsing string constants
    if (ch == '"') {
        lex_next(lex);
    }
    // parsing char constants
    if (ch == '\'') {
        lex_token_appendc(lextkn, ch);
        lex_next(lex);
        ch = lex_readc(lex);
        if (ch != '\\') {
            lex_token_appendc(lextkn, ch);
            lex_next(lex);
        }
        else {
            lex_token_appendc(lextkn, ch);
            lex_next(lex);
            ch = lex_readc(lex);
            switch (ch) {
            case 'a':
            case 'b':
            case 'f':
            case 'n':
            case 'r':
            case 'v':
            case '\\':
            case '\'':
            case '"':
            case '0':
                lex_token_appendc(lextkn, ch);
                lex_next(lex);
                break;
            default:
                return new_error("err: unknown escape.");
            }
        }
        ch = lex_readc(lex);
        if (ch == '\'') {
            lex_token_appendc(lextkn, ch);
        }
        else {
            return new_error("err: error character constant.");
        }
    }

    // parsing operators
    lex_token_appendc(lextkn, ch);
    lex_next(lex);
    if (ch < 42) {
        switch (ch) {
        case '(':
            lextkn->token_type = TOKEN_OP_LPARENTHESE;
            return NULL;
        case ')':
            lextkn->token_type = TOKEN_OP_RPARENTHESE;
            return NULL;
        case '&':
            ch = lex_readc(lex);
            if (ch == '&') {
                lex_token_appendc(lextkn, '&');
                lex_next(lex);
                lextkn->token_type = TOKEN_OP_LOGIC_AND;
                return NULL;
            }
            else if (ch == '=') {
                lex_token_appendc(lextkn, '=');
                lex_next(lex);
                lextkn->token_type = TOKEN_OP_ANDASSIGN;
                return NULL;
            }
            else {
                lextkn->token_type = TOKEN_OP_AND;
                return NULL;
            }
        case '!':
            if (lex_readc(lex) == '=') {
                lex_token_appendc(lextkn, '=');
                lex_next(lex);
                lextkn->token_type = TOKEN_OP_NOTEQ;
                return NULL;
            }
            else {
                lextkn->token_type = TOKEN_OP_NOT;
                return NULL;
            }
        case '%':
            if (lex_readc(lex) == '=') {
                lex_token_appendc(lextkn, '=');
                lex_next(lex);
                lextkn->token_type = TOKEN_OP_MODASSIGN;
                return NULL;
            }
            else {
                lextkn->token_type = TOKEN_OP_MOD;
                return NULL;
            }
        }
    }
    else if (ch < 47) {
        switch (ch) {
        case '.':
            lextkn->token_type = TOKEN_OP_SPOT;
            return NULL;
        case ',':
            lextkn->token_type = TOKEN_OP_COMMA;
            return NULL;
        case '+':
            ch = lex_readc(lex);
            if (ch == '+') {
                lex_token_appendc(lextkn, '+');
                lex_next(lex);
                lextkn->token_type = TOKEN_OP_INC;
                return NULL;
            }
            else if (ch == '=') {
                lex_token_appendc(lextkn, '=');
                lex_next(lex);
                lextkn->token_type = TOKEN_OP_ADDASSIGN;
                return NULL;
            }
            else {
                lextkn->token_type = TOKEN_OP_ADD;
                return NULL;
            }
        case '-':
            ch = lex_readc(lex);
            if (ch == '-') {
                lex_token_appendc(lextkn, '-');
                lex_next(lex);
                lextkn->token_type = TOKEN_OP_DEC;
                return NULL;
            }
            else if (ch == '=') {
                lex_token_appendc(lextkn, '=');
                lex_next(lex);
                lextkn->token_type = TOKEN_OP_SUBASSIGN;
                return NULL;
            }
            else {
                lextkn->token_type = TOKEN_OP_SUB;
                return NULL;
            }
        case '*':
            if (lex_readc(lex) == '=') {
                lex_token_appendc(lextkn, '=');
                lex_next(lex);
                lextkn->token_type = TOKEN_OP_MULASSIGN;
                return NULL;
            }
            else {
                lextkn->token_type = TOKEN_OP_MUL;
                return NULL;
            }
        }
    }
    else if (ch < 63) {
        switch (ch) {
        case '=':
            if (lex_readc(lex) == '=') {
                lex_token_appendc(lextkn, '=');
                lex_next(lex);
                lextkn->token_type = TOKEN_OP_EQ;
                return NULL;
            }
            else {
                lextkn->token_type = TOKEN_OP_ASSIGN;
                return NULL;
            }
        case '<':
            ch = lex_readc(lex);
            if (ch == '=') {
                lex_token_appendc(lextkn, '=');
                lex_next(lex);
                lextkn->token_type = TOKEN_OP_LE;
                return NULL;
            }
            else if (ch == '<') {
                lex_token_appendc(lextkn, '<');
                lex_next(lex);
                lextkn->token_type = TOKEN_OP_SHL;
                return NULL;
            }
            else {
                lextkn->token_type = TOKEN_OP_LT;
                return NULL;
            }
        case '>':
            ch = lex_readc(lex);
            if (ch == '=') {
                lex_token_appendc(lextkn, '=');
                lex_next(lex);
                lextkn->token_type = TOKEN_OP_GE;
                return NULL;
            }
            else if (ch == '>') {
                lex_token_appendc(lextkn, '>');
                lex_next(lex);
                lextkn->token_type = TOKEN_OP_SHR;
                return NULL;
            }
            else {
                lextkn->token_type = TOKEN_OP_GT;
                return NULL;
            }
        case ':':
            lextkn->token_type = TOKEN_OP_COLON;
            return NULL;
        case '/':
            ch = lex_readc(lex);
            if (ch == '=') {
                lex_token_appendc(lextkn, '=');
                lex_next(lex);
                lextkn->token_type = TOKEN_OP_DIVASSIGN;
                return NULL;
            }
            else if (ch == '/') {
                lex_token_appendc(lextkn, '/');
                lex_next(lex);
                lextkn->token_type = TOKEN_OP_ONELINE_CMT;
                // TODO: do some process for the single line comment...
                return NULL;
            }
            else {
                lextkn->token_type = TOKEN_OP_DIV;
                return NULL;
            }
        }
    }
    else if (ch < 124) {
        switch (ch) {
        case '{':
            lextkn->token_type = TOKEN_OP_LBRACE;
            return NULL;
        case '[':
            lextkn->token_type = TOKEN_OP_LBRACKET;
            return NULL;
        case ']':
            lextkn->token_type = TOKEN_OP_RBRACKET;
            return NULL;
        case '?':
            lextkn->token_type = TOKEN_OP_QUES;
            return NULL;
        case '^':
            if (lex_readc(lex) == '=') {
                lex_token_appendc(lextkn, '=');
                lex_next(lex);
                lextkn->token_type = TOKEN_OP_XORASSIGN;
                return NULL;
            }
            else {
                lextkn->token_type = TOKEN_OP_XOR;
                return NULL;
            }
        }
    }
    else {
        switch (ch) {
        case '}':
            lextkn->token_type = TOKEN_OP_RBRACE;
            return NULL;
        case '|':
            ch = lex_readc(lex);
            if (ch == '|') {
                lex_token_appendc(lextkn, '|');
                lex_next(lex);
                lextkn->token_type = TOKEN_OP_LOGIC_OR;
                return NULL;
            }
            else if (ch == '=') {
                lex_token_appendc(lextkn, '=');
                lex_next(lex);
                lextkn->token_type = TOKEN_OP_ORASSIGN;
                return NULL;
            }
            else {
                lextkn->token_type = TOKEN_OP_OR;
                return NULL;
            }
        case '~':
            lextkn->token_type = TOKEN_OP_NEG;
            return NULL;
        default:
            // unknown token and should report error.
            lextkn->token_type = TOKEN_UNKNOWN;
            return new_error("err: unknown token");
        }
    }
}

error lex_peek_token(lex_analyzer* lex, lex_token* lextkn) {

}
