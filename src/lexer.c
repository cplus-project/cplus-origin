/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "lexer.h"

static error err = NULL;

// you should always use this micro definition to increase the
// buffer's current index of the lexer.
#define lexerNext(lexer) \
lexer->i++;                              \
lexer->pos_col++;                        \
if (lexer->i >= lexer->buff_end_index) { \
    err = lexerReadFile(lexer);          \
    if (err != NULL) {                   \
        return err;                      \
    }                                    \
}

/****** methods of LexToken ******/

error lexTokenInit(LexToken* lextkn, int64 capacity) {
    if ((err = dynamicArrCharInit(&lextkn->token, capacity)) != NULL) {
        return err;
    }
    lextkn->token_len  = 0;
    lextkn->token_code = TOKEN_UNKNOWN;
    return NULL;
}

void lexTokenAppend(LexToken* lextkn, char* str, int64 len) {
    dynamicArrCharAppend(&lextkn->token, str, len);
    lextkn->token_len += len;
}

void lexTokenAppendc(LexToken* lextkn, char ch) {
    dynamicArrCharAppendc(&lextkn->token, ch);
    lextkn->token_len++;
}

char* lexTokenGetStr(LexToken* lextkn) {
    return dynamicArrCharGetStr(&lextkn->token);
}

void lexTokenClear(LexToken* lextkn) {
    dynamicArrCharClear(&lextkn->token);
    lextkn->token_len  = 0;
    lextkn->token_code = TOKEN_UNKNOWN;
}

void lexTokenDebug(LexToken* lextkn) {
    int32 code = lextkn->token_code;
    if (code == 100) {
        printf("identifier: %s\r\n", lexTokenGetStr(lextkn));
        return;
    }
    else if (200 <= code && code < 300) {
        switch (code) {
        case TOKEN_KEYWORD_IF:
            printf("keyword: if\r\n");
            return;
        case TOKEN_KEYWORD_EF:
            printf("keyword: ef\r\n");
            return;
        case TOKEN_KEYWORD_ELSE:
            printf("keyword: else\r\n");
            return;
        case TOKEN_KEYWORD_FOR:
            printf("keyword: for\r\n");
            return;
        case TOKEN_KEYWORD_BREAK:
            printf("keyword: break\r\n");
            return;
        case TOKEN_KEYWORD_CONTINUE:
            printf("keyword: continue\r\n");
            return;
        case TOKEN_KEYWORD_FUNC:
            printf("keyword: func\r\n");
            return;
        case TOKEN_KEYWORD_RETURN:
            printf("keyword: return\r\n");
            return;
        case TOKEN_KEYWORD_ERROR:
            printf("keyword: error\r\n");
            return;
        case TOKEN_KEYWORD_DEAL:
            printf("keyword: deal\r\n");
            return;
        case TOKEN_KEYWORD_CASE:
            printf("keyword: case\r\n");
            return;
        case TOKEN_KEYWORD_TYPE:
            printf("keyword: type\r\n");
            return;
        case TOKEN_KEYWORD_IN:
            printf("keyword: in\r\n");
            return;
        case TOKEN_KEYWORD_OT:
            printf("keyword: ot\r\n");
            return;
        case TOKEN_KEYWORD_INCLUDE:
            printf("keyword: include\r\n");
            return;
        case TOKEN_KEYWORD_MODULE:
            printf("keyword: module\r\n");
            return;
        case TOKEN_KEYWORD_DEFAULT:
            printf("keyword: default\r\n");
            return;
        case TOKEN_KEYWORD_SWITCH:
            printf("keyword: switch\r\n");
            return;
        }
    }
    else if (300 <= code && code < 400) {
        switch (code) {
        case TOKEN_CONST_INTEGER:
            printf("const-integer: %s\r\n", lexTokenGetStr(lextkn));
            return;
        case TOKEN_CONST_FLOAT:
            printf("const-float: %s\r\n"  , lexTokenGetStr(lextkn));
            return;
        case TOKEN_CONST_CHAR:
            printf("const-char: %s\r\n"   , lexTokenGetStr(lextkn));
            return;
        case TOKEN_CONST_STRING:
            printf("const-string: %s\r\n" , lexTokenGetStr(lextkn));
            return;
        }
    }
    else if (400 <= code && code < 500){
        printf("operator: %s\r\n", lexTokenGetStr(lextkn));
        return;
    }
    else {
        printf("unknown: %s\r\n", lexTokenGetStr(lextkn));
        return;
    }
}

void lexTokenDestroy(LexToken* lextkn) {
    dynamicArrCharDestroy(&lextkn->token);
}

/****** methods of Lexer ******/

error lexerInit(Lexer* lexer) {
    lexer->srcfile        = NULL;
    lexer->pos_file       = NULL;
    lexer->pos_line       = 1;
    lexer->pos_col        = 1;
    lexer->buff_end_index = 0;
    lexer->i              = 0;
    lexer->parse_lock     = false;
    if ((err = lexTokenInit(&lexer->lextkn, 255)) != NULL) {
        return err;
    }
    int16 j;
    for (j = 0; j < LEX_BUFF_SIZE; j++) {
        lexer->buffer[j] = 0;
    }
    return NULL;
}

error lexerOpenSrcFile(Lexer* lexer, char* file) {
    lexer->srcfile = fopen(file, "r");
    if (lexer->srcfile == NULL) {
        DynamicArrChar darr;
        dynamicArrCharInit(&darr, 255);
        dynamicArrCharAppend(&darr, "not found the source file: ", 27);
        dynamicArrCharAppend(&darr, file, strlen(file));
        char* errmsg = dynamicArrCharGetStr(&darr);
        dynamicArrCharDestroy(&darr);
        return new_error(errmsg);
    }
    lexer->pos_file = file;
    return NULL;
}

void lexerCloseSrcFile(Lexer* lexer) {
    if (lexer->srcfile == NULL) {
        printf("err: no file opened.\n");
        exit(EXIT_FAILURE);
    }
    fclose(lexer->srcfile);
    lexer->srcfile = NULL;
}

// read the source code from source file to buffer.
// when lexer->i >= lexer->buff_end_index, it means that the source codes
// now in the lexer->buffer are all processed completely. and now the lexical
// analyzer can read next LEX_BUFF_SIZE bytes source codes from the source
// file.
static error lexerReadFile(Lexer* lexer) {
    int64 read_len = fread(lexer->buffer, 1, LEX_BUFF_SIZE, lexer->srcfile);
    // process the end-of-file.
    if (feof(lexer->srcfile) != 0 && read_len == 0) {
        return NEW_ERROR_CODE(LEX_ERROR_EOF);
    }
    // process the read errors.
    if (ferror(lexer->srcfile) != 0 || read_len < 0) {
        return new_error("err: some error occur when parsing.");
    }
    lexer->buff_end_index = read_len;
    lexer->i = 0;
    return NULL;
}

static char lexerReadc(Lexer* lexer) {
    return lexer->buffer[lexer->i];
}

// used to parse scientific notation part of a number
// constant. you shoud use this as below:
//   ...
//   for (;;) {
//       char ch = lexerReadc(&lexer);
//       if (ch == 'e' || ch == 'E') {
//           lexerNext(lexer);
//           error err = lexerParseScientificNotation(&lex, lextkn);
//           if (err != NULL) {
//               return err;
//           }
//           else {
//               lextkn->token_code = TOKEN_CONST_NUMBER;
//               return NULL;
//           }
//       }
//   }
//   ...
static error lexerParseScientificNotation(Lexer* lexer, LexToken* lextkn) {
    lexTokenAppendc(lextkn, 'E');
    char ch = lexerReadc(lexer);
    if (ch == '-' || ch == '+') {
        lexTokenAppendc(lextkn, ch);
        lexerNext(lexer);
    }
    for (;;) {
        ch = lexerReadc(lexer);
        if ('0' <= ch && ch <= '9') {
            lexTokenAppendc(lextkn, ch);
            lexerNext(lexer);
        }
        else {
            return NULL;
        }
    }
}

error lexerParseToken(Lexer* lexer) {
    if (lexer->parse_lock == true) {
        return NULL;
    }

    char ch = '0';
    lexTokenClear(&lexer->lextkn);
    if (lexer->i >= lexer->buff_end_index) {
        if ((err = lexerReadFile(lexer)) != NULL) {
            return err;
        }
    }

    // skip the blank characters. all blank characters make no sense in c+.
    for (;;) {
        ch = lexerReadc(lexer);
        if (ch == ' ' || ch == '\t') {
            lexerNext(lexer);
        }
        else if (ch == '\r' || ch == '\n') {
            lexerNext(lexer);
            // counting the line numbers to make some preparations for
            // debuging and error/warning reporting.
            lexer->pos_line++;
            lexer->pos_col    = 1;
            lexer->parse_lock = true;

            lexer->lextkn.token_code = TOKEN_LINEFEED;
            lexer->lextkn.extra_info = EXTRA_INFO_EXPR_END;
            return NULL;
        }
        else {
            break;
        }
    }

    // parsing identifers and keywords
    if (('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') || ch == '_') {
        lexTokenAppendc(&lexer->lextkn, ch);
        lexerNext(lexer);
        for (;;) {
            ch = lexerReadc(lexer);
            if (('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') || ('0' <= ch && ch <= '9') || ch == '_') {
                lexTokenAppendc(&lexer->lextkn, ch);
                lexerNext(lexer);
            }
            else {
                char* token_content = lexTokenGetStr(&lexer->lextkn);
                if ('A' <= token_content[0] && token_content[0] <= 'Z') {
                    if (lexer->lextkn.token_len == 4 && strcmp(token_content, "Int8") == 0) {
                        lexer->lextkn.token_code = TOKEN_TYPE_INT8;
                        lexer->parse_lock = true;
                        return NULL;
                    }
                    else if (lexer->lextkn.token_len == 5 && strcmp(token_content, "Int16") == 0) {
                        lexer->lextkn.token_code = TOKEN_TYPE_INT16;
                        lexer->parse_lock = true;
                        return NULL;
                    }
                    else if (lexer->lextkn.token_len == 5 && strcmp(token_content, "Int32") == 0) {
                        lexer->lextkn.token_code = TOKEN_TYPE_INT32;
                        lexer->parse_lock = true;
                        return NULL;
                    }
                    else if (lexer->lextkn.token_len == 5 && strcmp(token_content, "Int64") == 0) {
                        lexer->lextkn.token_code = TOKEN_TYPE_INT64;
                        lexer->parse_lock = true;
                        return NULL;
                    }
                    else if (lexer->lextkn.token_len == 6 && strcmp(token_content, "String") == 0) {
                        lexer->lextkn.token_code = TOKEN_TYPE_STRING;
                        lexer->parse_lock = true;
                        return NULL;
                    }
                    else if (lexer->lextkn.token_len == 4 && strcmp(token_content, "Char") == 0) {
                        lexer->lextkn.token_code = TOKEN_TYPE_CHAR;
                        lexer->parse_lock = true;
                        return NULL;
                    }
                    else if (lexer->lextkn.token_len == 7 && strcmp(token_content, "Float64") == 0) {
                        lexer->lextkn.token_code = TOKEN_TYPE_FLOAT64;
                        lexer->parse_lock = true;
                        return NULL;
                    }
                    else if (lexer->lextkn.token_len == 7 && strcmp(token_content, "Float32") == 0) {
                        lexer->lextkn.token_code = TOKEN_TYPE_FLOAT32;
                        lexer->parse_lock = true;
                        return NULL;
                    }
                    else if (lexer->lextkn.token_len == 5 && strcmp(token_content, "Uint8") == 0) {
                        lexer->lextkn.token_code = TOKEN_TYPE_UINT8;
                        lexer->parse_lock = true;
                        return NULL;
                    }
                    else if (lexer->lextkn.token_len == 6 && strcmp(token_content, "Uint16") == 0) {
                        lexer->lextkn.token_code = TOKEN_TYPE_UINT16;
                        lexer->parse_lock = true;
                        return NULL;
                    }
                    else if (lexer->lextkn.token_len == 6 && strcmp(token_content, "Uint32") == 0) {
                        lexer->lextkn.token_code = TOKEN_TYPE_UINT32;
                        lexer->parse_lock = true;
                        return NULL;
                    }
                    else if (lexer->lextkn.token_len == 6 && strcmp(token_content, "Uint64") == 0) {
                        lexer->lextkn.token_code = TOKEN_TYPE_UINT64;
                        lexer->parse_lock = true;
                        return NULL;
                    }
                    else if (lexer->lextkn.token_len == 4 && strcmp(token_content, "Byte") == 0) {
                        lexer->lextkn.token_code = TOKEN_TYPE_BYTE;
                        lexer->parse_lock = true;
                        return NULL;
                    }
                    else if (lexer->lextkn.token_len == 9 && strcmp(token_content, "Complex64") == 0) {
                        lexer->lextkn.token_code = TOKEN_TYPE_COMPLEX64;
                        lexer->parse_lock = true;
                        return NULL;
                    }
                    else if (lexer->lextkn.token_len == 10 && strcmp(token_content, "Complex128") == 0) {
                        lexer->lextkn.token_code = TOKEN_TYPE_COMPLEX128;
                        lexer->parse_lock = true;
                        return NULL;
                    }
                }
                if ('b' <= token_content[0] && token_content[0] <= 'f') {
                    // for func fallthrough
                    if (lexer->lextkn.token_len == 3 && strcmp(token_content, "for") == 0) {
                        lexer->lextkn.token_code = TOKEN_KEYWORD_FOR;
                        lexer->parse_lock = true;
                        return NULL;
                    }
                    else if (lexer->lextkn.token_len == 4 && strcmp(token_content, "func") == 0) {
                        lexer->lextkn.token_code = TOKEN_KEYWORD_FUNC;
                        lexer->parse_lock = true;
                        return NULL;
                    }
                    else if (lexer->lextkn.token_len == 11 && strcmp(token_content, "fallthrough") == 0) {
                        lexer->lextkn.token_code = TOKEN_KEYWORD_FTHROUGH;
                        lexer->parse_lock = true;
                        return NULL;
                    }
                    // ef else
                    if (lexer->lextkn.token_len == 2 && strcmp(token_content, "ef") == 0) {
                        lexer->lextkn.token_code = TOKEN_KEYWORD_EF;
                        lexer->parse_lock = true;
                        return NULL;
                    }
                    else if (lexer->lextkn.token_len == 4 && strcmp(token_content, "else") == 0) {
                        lexer->lextkn.token_code = TOKEN_KEYWORD_ELSE;
                        lexer->parse_lock = true;
                        return NULL;
                    }
                    else if (lexer->lextkn.token_len == 5 && strcmp(token_content, "error") == 0) {
                        lexer->lextkn.token_code = TOKEN_KEYWORD_ERROR;
                        lexer->parse_lock = true;
                        return NULL;
                    }
                    // break
                    if (lexer->lextkn.token_len == 5 && strcmp(token_content, "break") == 0) {
                        lexer->lextkn.token_code = TOKEN_KEYWORD_BREAK;
                        lexer->parse_lock = true;
                        return NULL;
                    }
                    // deal
                    if (lexer->lextkn.token_len == 4 && strcmp(token_content, "deal") == 0) {
                        lexer->lextkn.token_code = TOKEN_KEYWORD_DEAL;
                        lexer->parse_lock = true;
                        return NULL;
                    }
                    else if (lexer->lextkn.token_len == 7 && strcmp(token_content, "default") == 0) {
                        lexer->lextkn.token_code = TOKEN_KEYWORD_DEFAULT;
                        lexer->parse_lock = true;
                        return NULL;
                    }
                    // case continue
                    if (lexer->lextkn.token_len == 4 && strcmp(token_content, "case") == 0) {
                        lexer->lextkn.token_code = TOKEN_KEYWORD_CASE;
                        lexer->parse_lock = true;
                        return NULL;
                    }
                    else if (lexer->lextkn.token_len == 8 && strcmp(token_content, "continue") == 0) {
                        lexer->lextkn.token_code = TOKEN_KEYWORD_CONTINUE;
                        lexer->parse_lock = true;
                        return NULL;
                    }
                }
                switch (token_content[0]) {
                case 'i': // if in
                    if (lexer->lextkn.token_len == 2 && strcmp(token_content, "if") == 0) {
                        lexer->lextkn.token_code = TOKEN_KEYWORD_IF;
                        lexer->parse_lock = true;
                        return NULL;
                    }
                    else if (lexer->lextkn.token_len == 2 && strcmp(token_content, "in") == 0) {
                        lexer->lextkn.token_code = TOKEN_KEYWORD_IN;
                        lexer->parse_lock = true;
                        return NULL;
                    }
                    else if (lexer->lextkn.token_len == 7 && strcmp(token_content, "include") == 0) {
                        lexer->lextkn.token_code = TOKEN_KEYWORD_INCLUDE;
                        lexer->parse_lock = true;
                        return NULL;
                    }
                case 't': // type
                    if (lexer->lextkn.token_len == 4 && strcmp(token_content, "type") == 0) {
                        lexer->lextkn.token_code = TOKEN_KEYWORD_TYPE;
                        lexer->parse_lock = true;
                        return NULL;
                    }
                case 'r': // return
                    if (lexer->lextkn.token_len == 6 && strcmp(token_content, "return") == 0) {
                        lexer->lextkn.token_code = TOKEN_KEYWORD_RETURN;
                        lexer->parse_lock = true;
                        return NULL;
                    }
                case 'o': // ot
                    if (lexer->lextkn.token_len == 2 && strcmp(token_content, "ot") == 0) {
                        lexer->lextkn.token_code = TOKEN_KEYWORD_OT;
                        lexer->parse_lock = true;
                        return NULL;
                    }
                case 's': // switch
                    if (lexer->lextkn.token_len == 6 && strcmp(token_content, "switch") == 0) {
                        lexer->lextkn.token_code = TOKEN_KEYWORD_SWITCH;
                        lexer->parse_lock = true;
                        return NULL;
                    }
                case 'm': // module
                    if (lexer->lextkn.token_len == 6 && strcmp(token_content, "module") == 0) {
                        lexer->lextkn.token_code = TOKEN_KEYWORD_MODULE;
                        lexer->parse_lock = true;
                        return NULL;
                    }
                }
                // variable or constant names
                lexer->lextkn.token_code = TOKEN_ID;
                lexer->parse_lock = true;
                return NULL;
            }
        }
    }

    // parsing integer constants and float constants
    if ('0' <= ch && ch <= '9') {
        char first = ch;
        lexTokenAppendc(&lexer->lextkn, ch);
        lexerNext(lexer);
        // distinguish the different number formats, such as:
        // (1) ordinary format
        // (2) 0b... or 0B...
        // (3) 0o... or 0O...
        // (4) 0d... or 0D...
        // (5) 0x... or 0X...
        if (first != '0') {
            for (;;) {
                ch = lexerReadc(lexer);
                // integer part
                if ('0' <= ch && ch <= '9') {
                    lexTokenAppendc(&lexer->lextkn, ch);
                    lexerNext(lexer);
                }
                // floating part
                else if (ch == '.') {
                    lexTokenAppendc(&lexer->lextkn, ch);
                    lexerNext(lexer);
                    for (;;) {
                        ch = lexerReadc(lexer);
                        if ('0' <= ch && ch <= '9') {
                            lexTokenAppendc(&lexer->lextkn, ch);
                            lexerNext(lexer);
                        }
                        else if (ch == 'e' || ch == 'E') { // scientific notation part
                            lexerNext(lexer);
                            if ((err = lexerParseScientificNotation(lexer, &lexer->lextkn)) != NULL) {
                                return err;
                            }
                            else {
                                lexer->lextkn.token_code = TOKEN_CONST_FLOAT;
                                lexer->parse_lock = true;
                                return NULL;
                            }
                        }
                        else {
                            lexer->lextkn.token_code = TOKEN_CONST_FLOAT;
                            lexer->parse_lock = true;
                            return NULL;
                        }
                    }
                }
                else if (ch == 'e' || ch == 'E') { // scientific notation part
                    lexerNext(lexer);
                    if ((err = lexerParseScientificNotation(lexer, &lexer->lextkn)) != NULL) {
                        return err;
                    }
                    else {
                        lexer->lextkn.token_code = TOKEN_CONST_INTEGER;
                        lexer->parse_lock = true;
                        return NULL;
                    }
                }
                else {
                    lexer->lextkn.token_code = TOKEN_CONST_INTEGER;
                    lexer->parse_lock = true;
                    return NULL;
                }
            }
        }
        else {
            ch = lexerReadc(lexer);
            switch (ch) {
            case 'x':
            case 'X': // hex format
                lexTokenAppendc(&lexer->lextkn, ch);
                lexerNext(lexer);
                for (;;) {
                    ch = lexerReadc(lexer);
                    if (('0' <= ch && ch <= '9') || ('A' <= ch && ch <= 'F') || ('a' <= ch && ch <= 'f')) {
                        lexTokenAppendc(&lexer->lextkn, ch);
                        lexerNext(lexer);
                    }
                    else {
                        lexer->lextkn.token_code = TOKEN_CONST_INTEGER;
                        lexer->parse_lock = true;
                        return NULL;
                    }
                }
                break;
            case 'b':
            case 'B': // binary format
                lexerNext(lexer);
                lexTokenClear(&lexer->lextkn);
                for (;;) {
                    ch = lexerReadc(lexer);
                    if (ch == '0' || ch == '1') {
                        lexTokenAppendc(&lexer->lextkn, ch);
                        lexerNext(lexer);
                    }
                    else {
                        char* decimal = conv_itoa(conv_binary_to_decimal(lexTokenGetStr(&lexer->lextkn), lexer->lextkn.token_len));
                        lexTokenClear(&lexer->lextkn);
                        lexTokenAppend(&lexer->lextkn, decimal, strlen(decimal));
                        lexer->lextkn.token_code = TOKEN_CONST_INTEGER;
                        lexer->parse_lock = true;
                        return NULL;
                    }
                }
                break;
            case 'o':
            case 'O': // octal format
                lexerNext(lexer);
                for (;;) {
                    ch = lexerReadc(lexer);
                    if ('0' <= ch && ch <= '7') {
                        lexTokenAppendc(&lexer->lextkn, ch);
                        lexerNext(lexer);
                    }
                    else {
                        lexer->lextkn.token_code = TOKEN_CONST_INTEGER;
                        lexer->parse_lock = true;
                        return NULL;
                    }
                }
                break;
            case 'd':
            case 'D': // decimal format
                lexerNext(lexer);
                lexTokenClear(&lexer->lextkn);
                for (;;) {
                    ch = lexerReadc(lexer);
                    if ('0' <= ch && ch <= '9') {
                        lexTokenAppendc(&lexer->lextkn, ch);
                        lexerNext(lexer);
                    }
                    else {
                        lexer->lextkn.token_code = TOKEN_CONST_INTEGER;
                        lexer->parse_lock = true;
                        return NULL;
                    }
                }
                break;
            }
        }
    }

    // parsing string constants
    if (ch == '"') {
        lexerNext(lexer);
        for (;;) {
            ch = lexerReadc(lexer);
            if (ch != '"') {
                lexTokenAppendc(&lexer->lextkn, ch);
                lexerNext(lexer);
            }
            else {
                lexerNext(lexer);
                lexer->lextkn.token_code = TOKEN_CONST_STRING;
                lexer->parse_lock = true;
                return NULL;
            }
        }
    }

    // parsing char constants
    if (ch == '\'') {
        lexerNext(lexer);
        ch = lexerReadc(lexer);
        if (ch != '\\') {
            uint8 bytes = utf8_calcu_bytes(ch);
            switch (bytes) {
            case 1:
                lexTokenAppendc(&lexer->lextkn, ch);
                lexerNext(lexer);
                break;
            case 2:
                lexTokenAppendc(&lexer->lextkn, ch);
                lexerNext(lexer);
                ch = lexerReadc(lexer);
                lexTokenAppendc(&lexer->lextkn, ch);
                lexerNext(lexer);
                break;
            case 3:
                lexTokenAppendc(&lexer->lextkn, ch);
                lexerNext(lexer);
                ch = lexerReadc(lexer);
                lexTokenAppendc(&lexer->lextkn, ch);
                lexerNext(lexer);
                ch = lexerReadc(lexer);
                lexTokenAppendc(&lexer->lextkn, ch);
                lexerNext(lexer);
                break;
            case 4:
                lexTokenAppendc(&lexer->lextkn, ch);
                lexerNext(lexer);
                ch = lexerReadc(lexer);
                lexTokenAppendc(&lexer->lextkn, ch);
                lexerNext(lexer);
                ch = lexerReadc(lexer);
                lexTokenAppendc(&lexer->lextkn, ch);
                lexerNext(lexer);
                ch = lexerReadc(lexer);
                lexTokenAppendc(&lexer->lextkn, ch);
                lexerNext(lexer);
                break;
            default:
                return new_error("err: invalid UTF-8 code.");
            }
        }
        else {
            lexerNext(lexer);
            ch = lexerReadc(lexer);
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
                lexTokenAppendc(&lexer->lextkn, ch);
                lexerNext(lexer);
                break;
            default:
                return new_error("err: unknown escape.");
            }
        }
        ch = lexerReadc(lexer);
        if (ch == '\'') {
            lexer->lextkn.token_code = TOKEN_CONST_CHAR;
            lexer->parse_lock = true;
            return NULL;
        }
        else {
            return new_error("err: need a single quotation to close the character literal.");
        }
    }

    // parsing operators
    lexTokenAppendc(&lexer->lextkn, ch);
    lexerNext(lexer);
    if (ch < 42) {
        switch (ch) {
        case '(':
            lexer->lextkn.token_code = TOKEN_OP_LPARENTHESE;
            lexer->parse_lock = true;
            return NULL;
        case ')':
            lexer->lextkn.token_code = TOKEN_OP_RPARENTHESE;
            lexer->lextkn.extra_info = EXTRA_INFO_EXPR_END;
            lexer->parse_lock = true;
            return NULL;
        case '&':
            ch = lexerReadc(lexer);
            if (ch == '&') {
                lexTokenAppendc(&lexer->lextkn, '&');
                lexerNext(lexer);
                lexer->lextkn.token_code = TOKEN_OP_LOGIC_AND;
                lexer->lextkn.extra_info = EXTRA_INFO_OP_BINARY;
                lexer->parse_lock = true;
                return NULL;
            }
            else if (ch == '=') {
                lexTokenAppendc(&lexer->lextkn, '=');
                lexerNext(lexer);
                lexer->lextkn.token_code = TOKEN_OP_ANDASSIGN;
                lexer->parse_lock = true;
                return NULL;
            }
            else {
                lexer->lextkn.token_code = TOKEN_OP_AND;
                lexer->lextkn.extra_info = EXTRA_INFO_OP_BINARY;
                lexer->parse_lock = true;
                return NULL;
            }
        case '$':
            lexer->lextkn.token_code = TOKEN_OP_DEREFER;
            lexer->lextkn.extra_info = EXTRA_INFO_OP_LUNARY;
            lexer->parse_lock = true;
            return NULL;
        case '!':
            if (lexerReadc(lexer) == '=') {
                lexTokenAppendc(&lexer->lextkn, '=');
                lexerNext(lexer);
                lexer->lextkn.token_code = TOKEN_OP_NOTEQ;
                lexer->lextkn.extra_info = EXTRA_INFO_OP_BINARY;
                lexer->parse_lock = true;
                return NULL;
            }
            else {
                lexer->lextkn.token_code = TOKEN_OP_NOT;
                lexer->lextkn.extra_info = EXTRA_INFO_OP_LUNARY;
                lexer->parse_lock = true;
                return NULL;
            }
        case '%':
            if (lexerReadc(lexer) == '=') {
                lexTokenAppendc(&lexer->lextkn, '=');
                lexerNext(lexer);
                lexer->lextkn.token_code = TOKEN_OP_MODASSIGN;
                lexer->parse_lock = true;
                return NULL;
            }
            else {
                lexer->lextkn.token_code = TOKEN_OP_MOD;
                lexer->lextkn.extra_info = EXTRA_INFO_OP_BINARY;
                lexer->parse_lock = true;
                return NULL;
            }
        }
    }
    else if (ch < 47) {
        switch (ch) {
        case '.':
            lexer->lextkn.token_code = TOKEN_OP_SPOT;
            lexer->lextkn.extra_info = EXTRA_INFO_OP_BINARY;
            lexer->parse_lock = true;
            return NULL;
        case ',':
            lexer->lextkn.token_code = TOKEN_OP_COMMA;
            lexer->lextkn.extra_info = EXTRA_INFO_EXPR_END;
            lexer->parse_lock = true;
            return NULL;
        case '+':
            ch = lexerReadc(lexer);
            if (ch == '+') {
                lexTokenAppendc(&lexer->lextkn, '+');
                lexerNext(lexer);
                lexer->lextkn.token_code = TOKEN_OP_INC;
                lexer->lextkn.extra_info = EXTRA_INFO_OP_RUNARY;
                lexer->parse_lock = true;
                return NULL;
            }
            else if (ch == '=') {
                lexTokenAppendc(&lexer->lextkn, '=');
                lexerNext(lexer);
                lexer->lextkn.token_code = TOKEN_OP_ADDASSIGN;
                lexer->parse_lock = true;
                return NULL;
            }
            else {
                lexer->lextkn.token_code = TOKEN_OP_ADD;
                lexer->lextkn.extra_info = EXTRA_INFO_OP_BINARY;
                lexer->parse_lock = true;
                return NULL;
            }
        case '-':
            ch = lexerReadc(lexer);
            if (ch == '-') {
                lexTokenAppendc(&lexer->lextkn, '-');
                lexerNext(lexer);
                lexer->lextkn.token_code = TOKEN_OP_DEC;
                lexer->lextkn.extra_info = EXTRA_INFO_OP_RUNARY;
                lexer->parse_lock = true;
                return NULL;
            }
            else if (ch == '=') {
                lexTokenAppendc(&lexer->lextkn, '=');
                lexerNext(lexer);
                lexer->lextkn.token_code = TOKEN_OP_SUBASSIGN;
                lexer->parse_lock = true;
                return NULL;
            }
            else {
                lexer->lextkn.token_code = TOKEN_OP_SUB;
                lexer->lextkn.extra_info = EXTRA_INFO_OP_BINARY;
                lexer->parse_lock = true;
                return NULL;
            }
        case '*':
            if (lexerReadc(lexer) == '=') {
                lexTokenAppendc(&lexer->lextkn, '=');
                lexerNext(lexer);
                lexer->lextkn.token_code = TOKEN_OP_MULASSIGN;
                lexer->parse_lock = true;
                return NULL;
            }
            else {
                lexer->lextkn.token_code = TOKEN_OP_MUL;
                lexer->lextkn.extra_info = EXTRA_INFO_OP_BINARY;
                lexer->parse_lock = true;
                return NULL;
            }
        }
    }
    else if (ch < 63) {
        switch (ch) {
        case '=':
            if (lexerReadc(lexer) == '=') {
                lexTokenAppendc(&lexer->lextkn, '=');
                lexerNext(lexer);
                lexer->lextkn.token_code = TOKEN_OP_EQ;
                lexer->lextkn.extra_info = EXTRA_INFO_OP_BINARY;
                lexer->parse_lock = true;
                return NULL;
            }
            else {
                lexer->lextkn.token_code = TOKEN_OP_ASSIGN;
                lexer->parse_lock = true;
                return NULL;
            }
        case '<':
            ch = lexerReadc(lexer);
            if (ch == '=') {
                lexTokenAppendc(&lexer->lextkn, '=');
                lexerNext(lexer);
                lexer->lextkn.token_code = TOKEN_OP_LE;
                lexer->lextkn.extra_info = EXTRA_INFO_OP_BINARY;
                lexer->parse_lock = true;
                return NULL;
            }
            else if (ch == '<') {
                lexTokenAppendc(&lexer->lextkn, '<');
                lexerNext(lexer);
                lexer->lextkn.token_code = TOKEN_OP_SHL;
                lexer->lextkn.extra_info = EXTRA_INFO_OP_BINARY;
                lexer->parse_lock = true;
                return NULL;
            }
            else {
                lexer->lextkn.token_code = TOKEN_OP_LT;
                lexer->lextkn.extra_info = EXTRA_INFO_OP_BINARY;
                lexer->parse_lock = true;
                return NULL;
            }
        case '>':
            ch = lexerReadc(lexer);
            if (ch == '=') {
                lexTokenAppendc(&lexer->lextkn, '=');
                lexerNext(lexer);
                lexer->lextkn.token_code = TOKEN_OP_GE;
                lexer->lextkn.extra_info = EXTRA_INFO_OP_BINARY;
                lexer->parse_lock = true;
                return NULL;
            }
            else if (ch == '>') {
                lexTokenAppendc(&lexer->lextkn, '>');
                lexerNext(lexer);
                lexer->lextkn.token_code = TOKEN_OP_SHR;
                lexer->lextkn.extra_info = EXTRA_INFO_OP_BINARY;
                lexer->parse_lock = true;
                return NULL;
            }
            else {
                lexer->lextkn.token_code = TOKEN_OP_GT;
                lexer->lextkn.extra_info = EXTRA_INFO_OP_BINARY;
                lexer->parse_lock = true;
                return NULL;
            }
        case ':':
            lexer->lextkn.token_code = TOKEN_OP_COLON;
            lexer->parse_lock = true;
            return NULL;
        case '/':
            ch = lexerReadc(lexer);
            if (ch == '/') {
                // parsing the single line comment
                lexTokenClear(&lexer->lextkn);
                lexerNext(lexer);
                for (;;) {
                    ch = lexerReadc(lexer);
                    if (ch != '\r' && ch != '\n') {
                        lexTokenAppendc(&lexer->lextkn, ch);
                        lexerNext(lexer);
                    }
                    else {
                        lexer->lextkn.token_code = TOKEN_OP_SINGLE_CMT;
                        lexer->parse_lock = true;
                        return NULL;
                    }
                }
            }
            else if (ch == '*') {
                // parsing the multiple line comment
                lexTokenClear(&lexer->lextkn);
                lexerNext(lexer);
                int embed = 0;
                for (;;) {
                    ch = lexerReadc(lexer);
                    if (ch == '*') {
                        lexerNext(lexer);
                        if (lexerReadc(lexer) == '/') {
                            if (embed <= 0) {
                                lexer->lextkn.token_code = TOKEN_OP_MULTIL_CMT;
                                lexer->parse_lock = true;
                                return NULL;
                            }
                            else {
                                embed--;
                            }
                        }
                    }
                    else if (ch == '/') {
                        lexerNext(lexer);
                        if (lexerReadc(lexer) == '*') {
                            embed++;
                            lexerNext(lexer);
                        }
                    }
                }
            }
            else if (ch == '=') {
                lexTokenAppendc(&lexer->lextkn, '=');
                lexerNext(lexer);
                lexer->lextkn.token_code = TOKEN_OP_DIVASSIGN;
                lexer->parse_lock = true;
                return NULL;
            }
            else {
                lexer->lextkn.token_code = TOKEN_OP_DIV;
                lexer->lextkn.extra_info = EXTRA_INFO_OP_BINARY;
                lexer->parse_lock = true;
                return NULL;
            }
        
        case ';':
            lexer->lextkn.token_code = TOKEN_OP_SEMC;
            lexer->lextkn.extra_info = EXTRA_INFO_EXPR_END;
            lexer->parse_lock = true;
            return NULL;
        }
    }
    else if (ch < 124) {
        switch (ch) {
        case '{':
            lexer->lextkn.token_code = TOKEN_OP_LBRACE;
            lexer->lextkn.extra_info = EXTRA_INFO_EXPR_END;
            lexer->parse_lock = true;
            return NULL;
        case '[':
            lexer->lextkn.token_code = TOKEN_OP_LBRACKET;
            lexer->parse_lock = true;
            return NULL;
        case ']':
            lexer->lextkn.token_code = TOKEN_OP_RBRACKET;
            lexer->lextkn.extra_info = EXTRA_INFO_EXPR_END;
            lexer->parse_lock = true;
            return NULL;
        case '@':
            lexer->lextkn.token_code = TOKEN_OP_GETADDR;
            lexer->lextkn.extra_info = EXTRA_INFO_OP_LUNARY;
            lexer->parse_lock = true;
            return NULL;
        case '?':
            lexer->lextkn.token_code = TOKEN_OP_QUES;
            lexer->parse_lock = true;
            return NULL;
        case '^':
            if (lexerReadc(lexer) == '=') {
                lexTokenAppendc(&lexer->lextkn, '=');
                lexerNext(lexer);
                lexer->lextkn.token_code = TOKEN_OP_XORASSIGN;
                lexer->parse_lock = true;
                return NULL;
            }
            else {
                lexer->lextkn.token_code = TOKEN_OP_XOR;
                lexer->lextkn.extra_info = EXTRA_INFO_OP_BINARY;
                lexer->parse_lock = true;
                return NULL;
            }
        }
    }
    else {
        switch (ch) {
        case '}':
            lexer->lextkn.token_code = TOKEN_OP_RBRACE;
            lexer->parse_lock = true;
            return NULL;
        case '|':
            ch = lexerReadc(lexer);
            if (ch == '|') {
                lexTokenAppendc(&lexer->lextkn, '|');
                lexerNext(lexer);
                lexer->lextkn.token_code = TOKEN_OP_LOGIC_OR;
                lexer->lextkn.extra_info = EXTRA_INFO_OP_BINARY;
                lexer->parse_lock = true;
                return NULL;
            }
            else if (ch == '=') {
                lexTokenAppendc(&lexer->lextkn, '=');
                lexerNext(lexer);
                lexer->lextkn.token_code = TOKEN_OP_ORASSIGN;
                lexer->parse_lock = true;
                return NULL;
            }
            else {
                lexer->lextkn.token_code = TOKEN_OP_OR;
                lexer->lextkn.extra_info = EXTRA_INFO_OP_BINARY;
                lexer->parse_lock = true;
                return NULL;
            }
        case '~':
            lexer->lextkn.token_code = TOKEN_OP_SUB;
            lexer->lextkn.extra_info = EXTRA_INFO_OP_BINARY;
            lexer->parse_lock = true;
            return NULL;
        }
    }

    lexer->lextkn.token_code = TOKEN_UNKNOWN;
    return new_error("err: unknown token");
}

LexToken* lexerReadToken(Lexer* lexer) {
    return &lexer->lextkn;
}

// continue to parse the next token
void lexerNextToken(Lexer* lexer) {
    lexer->parse_lock = false;
}

void lexerDestroy(Lexer* lexer) {
    lexTokenDestroy(&lexer->lextkn);
    lexerCloseSrcFile(lexer);
}

#undef lex_next
