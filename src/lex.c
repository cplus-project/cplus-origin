/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "lex.h"

/****** methods of lex_token ******/

// you should always use the lex_idx_inc(lex->i) to
// increase the buffer's current index of the lexical
// analyzer.
#define lex_next(lex) \
lex->i++;                            \
lex->col++;                          \
if (lex->i >= lex->buff_end_index) { \
    error err = lex_read_file(lex);  \
    if (err != NULL) {               \
        return err;                  \
    }                                \
}

error lex_token_init(lex_token* lextkn, int64 capacity) {
    error err = dynamicarr_char_init(&lextkn->token, capacity);
    if (err != NULL) {
        return err;
    }
    lextkn->token_len  = 0;
    lextkn->token_type = TOKEN_UNKNOWN;
    return NULL;
}

void lex_token_append(lex_token* lextkn, char* str, int64 len) {
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

void lex_token_debug(lex_token* lextkn) {
    int32 type = lextkn->token_type;
    if (type == 100) {
        printf("identifier: %s\r\n", lex_token_getstr(lextkn));
        return;
    }
    else if (200 <= type && type < 300) {
        switch (type) {
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
    else if (300 <= type && type < 400) {
        switch (type) {
        case TOKEN_CONST_INTEGER:
            printf("const-integer: %s\r\n", lex_token_getstr(lextkn));
            return;
        case TOKEN_CONST_FLOAT:
            printf("const-float: %s\r\n", lex_token_getstr(lextkn));
            return;
        case TOKEN_CONST_CHAR:
            printf("const-char: %s\r\n", lex_token_getstr(lextkn));
            return;
        case TOKEN_CONST_STRING:
            printf("const-string: %s\r\n", lex_token_getstr(lextkn));
            return;
        }
    }
    else if (400 <= type && type < 500){
        printf("operator: %s\r\n", lex_token_getstr(lextkn));
        return;
    }
    else {
        printf("unknown: %s\r\n", lex_token_getstr(lextkn));
        return;
    }
}

void lex_token_destroy(lex_token* lextkn) {
    dynamicarr_char_destroy(&lextkn->token);
}

/****** methods of lex_token_stack ******/

void lex_token_stack_init(lex_token_stack* lexstk) {
    lexstk->top = NULL;
}

void lex_token_stack_push(lex_token_stack* lexstk, char* token, int16 token_type) {
    lex_token_stack_node* create = (lex_token_stack_node*)mem_alloc(sizeof(lex_token_stack_node));
    create->token      = token;
    create->token_type = token_type;
    create->next       = NULL;
    if (lexstk->top != NULL) {
        create->next = lexstk->top;
        lexstk->top  = create;
    }
    else {
        lexstk->top = create;
    }
}

// return true if the stack is empty.
bool lex_token_stack_isempty(lex_token_stack* lexstk) {
    if (lexstk->top == NULL) {
        return true;
    }
    return false;
}

char* lex_token_stack_top_token(lex_token_stack* lexstk) {
    return lexstk->top->token;
}

int16 lex_token_stack_top_type(lex_token_stack* lexstk) {
    return lexstk->top->token_type;
}

void lex_token_stack_pop(lex_token_stack* lexstk) {
    lex_token_stack_node* temp = lexstk->top;
    lexstk->top = lexstk->top->next;
    mem_free(temp);
}

void lex_token_stack_destroy(lex_token_stack* lexstk) {
    lex_token_stack_node* temp;
    for (;;) {
        if (lexstk->top == NULL) {
            return;
        }
        temp = lexstk->top;
        lexstk->top = lexstk->top->next;
        mem_free(temp);
    }
}

/****** methods of lex_analyzer ******/

error lex_init(lex_analyzer* lex) {
    lex->srcfile        = NULL;
    lex->line           = 1;
    lex->col            = 1;
    lex->buff_end_index = 0;
    lex->i              = 0;
    lex->parse_lock     = false;
    error err = lex_token_init(&lex->lextkn, 255);
    if (err != NULL) {
        return err;
    }
    int16 j;
    for (j = 0; j < LEX_BUFF_SIZE; j++) {
        lex->buffer[j] = 0;
    }
    return NULL;
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
    lex->srcfile = NULL;
}

// read the source code from source file to buffer.
// when lex->i >= lex->buff_end_index, it means that the source codes
// now in the lex->buffer are all processed completely. and now the lexical
// analyzer can read next LEX_BUFF_SIZE bytes source codes from the source
// file.
static error lex_read_file(lex_analyzer* lex) {
    int64 read_len = fread(lex->buffer, 1, LEX_BUFF_SIZE, lex->srcfile);
    // process the end-of-file.
    if (feof(lex->srcfile) != 0 && read_len == 0) {
        return new_error("EOF");
    }
    // process the read errors.
    if (ferror(lex->srcfile) != 0 || read_len < 0) {
        return new_error("err: some error occur when parsing.");
    }
    lex->buff_end_index = read_len;
    lex->i = 0;
    return NULL;
}

static char lex_readc(lex_analyzer* lex) {
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
static error lex_parse_scientific_notation(lex_analyzer* lex, lex_token* lextkn) {
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
            return NULL;
        }
    }
}

error lex_parse_token(lex_analyzer* lex) {
    if (lex->parse_lock == true) {
        return NULL;
    }

    char ch = '0';
    lex_token_clear(&lex->lextkn);
    if (lex->i >= lex->buff_end_index) {
        error err = lex_read_file(lex);
        if (err != NULL) {
            return err;
        }
    }

    // skip the blank characters. all blank characters make no sense in c+.
    for (;;) {
        ch = lex_readc(lex);
        if (ch == ' ' || ch == '\t') {
            lex_next(lex);
        }
        else if (ch == '\r' || ch == '\n') {
            lex_next(lex);
            // counting the line numbers to make some preparations for
            // debuging and error/warning reporting.
            lex->line++;
            lex->col = 1;
            lex->lextkn.token_type = TOKEN_NEXT_LINE;
            lex->parse_lock = true;
            return NULL;
        }
        else {
            break;
        }
    }

    // parsing identifers and keywords
    if (('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') || ch == '_') {
        lex_token_appendc(&lex->lextkn, ch);
        lex_next(lex);
        for (;;) {
            ch = lex_readc(lex);
            if (('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') || ('0' <= ch && ch <= '9') || ch == '_') {
                lex_token_appendc(&lex->lextkn, ch);
                lex_next(lex);
            }
            else {
                char* token_content = lex_token_getstr(&lex->lextkn);
                if ('b' <= token_content[0] && token_content[0] <= 'f') {
                    // for func
                    if (lex->lextkn.token_len == 3 && strcmp(token_content, "for") == 0) {
                        lex->lextkn.token_type = TOKEN_KEYWORD_FOR;
                        lex->parse_lock = true;
                        return NULL;
                    }
                    else if (lex->lextkn.token_len == 4 && strcmp(token_content, "func") == 0) {
                        lex->lextkn.token_type = TOKEN_KEYWORD_FUNC;
                        lex->parse_lock = true;
                        return NULL;
                    }
                    // ef else
                    if (lex->lextkn.token_len == 2 && strcmp(token_content, "ef") == 0) {
                        lex->lextkn.token_type = TOKEN_KEYWORD_EF;
                        lex->parse_lock = true;
                        return NULL;
                    }
                    else if (lex->lextkn.token_len == 4 && strcmp(token_content, "else") == 0) {
                        lex->lextkn.token_type = TOKEN_KEYWORD_ELSE;
                        lex->parse_lock = true;
                        return NULL;
                    }
                    else if (lex->lextkn.token_len == 5 && strcmp(token_content, "error") == 0) {
                        lex->lextkn.token_type = TOKEN_KEYWORD_ERROR;
                        lex->parse_lock = true;
                        return NULL;
                    }
                    // break
                    if (lex->lextkn.token_len == 5 && strcmp(token_content, "break") == 0) {
                        lex->lextkn.token_type = TOKEN_KEYWORD_BREAK;
                        lex->parse_lock = true;
                        return NULL;
                    }
                    // deal
                    if (lex->lextkn.token_len == 4 && strcmp(token_content, "deal") == 0) {
                        lex->lextkn.token_type = TOKEN_KEYWORD_DEAL;
                        lex->parse_lock = true;
                        return NULL;
                    }
                    else if (lex->lextkn.token_len == 7 && strcmp(token_content, "default") == 0) {
                        lex->lextkn.token_type = TOKEN_KEYWORD_DEFAULT;
                        lex->parse_lock = true;
                        return NULL;
                    }
                    // case continue
                    if (lex->lextkn.token_len == 4 && strcmp(token_content, "case") == 0) {
                        lex->lextkn.token_type = TOKEN_KEYWORD_CASE;
                        lex->parse_lock = true;
                        return NULL;
                    }
                    else if (lex->lextkn.token_len == 8 && strcmp(token_content, "continue") == 0) {
                        lex->lextkn.token_type = TOKEN_KEYWORD_CONTINUE;
                        lex->parse_lock = true;
                        return NULL;
                    }
                }
                switch (token_content[0]) {
                case 'i': // if in
                    if (lex->lextkn.token_len == 2 && strcmp(token_content, "if") == 0) {
                        lex->lextkn.token_type = TOKEN_KEYWORD_IF;
                        lex->parse_lock = true;
                        return NULL;
                    }
                    else if (lex->lextkn.token_len == 2 && strcmp(token_content, "in") == 0) {
                        lex->lextkn.token_type = TOKEN_KEYWORD_IN;
                        lex->parse_lock = true;
                        return NULL;
                    }
                    else if (lex->lextkn.token_len == 7 && strcmp(token_content, "include") == 0) {
                        lex->lextkn.token_type = TOKEN_KEYWORD_INCLUDE;
                        lex->parse_lock = true;
                        return NULL;
                    }
                case 't': // type
                    if (lex->lextkn.token_len == 4 && strcmp(token_content, "type") == 0) {
                        lex->lextkn.token_type = TOKEN_KEYWORD_TYPE;
                        lex->parse_lock = true;
                        return NULL;
                    }
                case 'r': // return
                    if (lex->lextkn.token_len == 6 && strcmp(token_content, "return") == 0) {
                        lex->lextkn.token_type = TOKEN_KEYWORD_RETURN;
                        lex->parse_lock = true;
                        return NULL;
                    }
                case 'o': // ot
                    if (lex->lextkn.token_len == 2 && strcmp(token_content, "ot") == 0) {
                        lex->lextkn.token_type = TOKEN_KEYWORD_OT;
                        lex->parse_lock = true;
                        return NULL;
                    }
                case 's': // switch
                    if (lex->lextkn.token_len == 6 && strcmp(token_content, "switch") == 0) {
                        lex->lextkn.token_type = TOKEN_KEYWORD_SWITCH;
                        lex->parse_lock = true;
                        return NULL;
                    }
                case 'm': // module
                    if (lex->lextkn.token_len == 6 && strcmp(token_content, "module") == 0) {
                        lex->lextkn.token_type = TOKEN_KEYWORD_MODULE;
                        lex->parse_lock = true;
                        return NULL;
                    }
                }
                // variable or constant names
                lex->lextkn.token_type = TOKEN_ID;
                lex->parse_lock = true;
                return NULL;
            }
        }
    }

    // parsing integer constants and float constants
    if ('0' <= ch && ch <= '9') {
        char first = ch;
        lex_token_appendc(&lex->lextkn, ch);
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
                    lex_token_appendc(&lex->lextkn, ch);
                    lex_next(lex);
                }
                // floating part
                else if (ch == '.') {
                    lex_token_appendc(&lex->lextkn, ch);
                    lex_next(lex);
                    for (;;) {
                        ch = lex_readc(lex);
                        if ('0' <= ch && ch <= '9') {
                            lex_token_appendc(&lex->lextkn, ch);
                            lex_next(lex);
                        }
                        else if (ch == 'e' || ch == 'E') { // scientific notation part
                            lex_next(lex);
                            error err = lex_parse_scientific_notation(lex, &lex->lextkn);
                            if (err != NULL) {
                                return err;
                            }
                            else {
                                lex->lextkn.token_type = TOKEN_CONST_FLOAT;
                                lex->parse_lock = true;
                                return NULL;
                            }
                        }
                        else {
                            lex->lextkn.token_type = TOKEN_CONST_FLOAT;
                            lex->parse_lock = true;
                            return NULL;
                        }
                    }
                }
                else if (ch == 'e' || ch == 'E') { // scientific notation part
                    lex_next(lex);
                    error err = lex_parse_scientific_notation(lex, &lex->lextkn);
                    if (err != NULL) {
                        return err;
                    }
                    else {
                        lex->lextkn.token_type = TOKEN_CONST_INTEGER;
                        lex->parse_lock = true;
                        return NULL;
                    }
                }
                else {
                    lex->lextkn.token_type = TOKEN_CONST_INTEGER;
                    lex->parse_lock = true;
                    return NULL;
                }
            }
        }
        else {
            ch = lex_readc(lex);
            switch (ch) {
            case 'x':
            case 'X': // hex format
                lex_token_appendc(&lex->lextkn, ch);
                lex_next(lex);
                for (;;) {
                    ch = lex_readc(lex);
                    if (('0' <= ch && ch <= '9') || ('A' <= ch && ch <= 'F') || ('a' <= ch && ch <= 'f')) {
                        lex_token_appendc(&lex->lextkn, ch);
                        lex_next(lex);
                    }
                    else {
                        lex->lextkn.token_type = TOKEN_CONST_INTEGER;
                        lex->parse_lock = true;
                        return NULL;
                    }
                }
                break;
            case 'b':
            case 'B': // binary format
                lex_next(lex);
                lex_token_clear(&lex->lextkn);
                for (;;) {
                    ch = lex_readc(lex);
                    if (ch == '0' || ch == '1') {
                        lex_token_appendc(&lex->lextkn, ch);
                        lex_next(lex);
                    }
                    else {
                        char* decimal = conv_itoa(conv_binary_to_decimal(lex_token_getstr(&lex->lextkn), lex->lextkn.token_len));
                        lex_token_clear(&lex->lextkn);
                        lex_token_append(&lex->lextkn, decimal, strlen(decimal));
                        lex->lextkn.token_type = TOKEN_CONST_INTEGER;
                        lex->parse_lock = true;
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
                        lex_token_appendc(&lex->lextkn, ch);
                        lex_next(lex);
                    }
                    else {
                        lex->lextkn.token_type = TOKEN_CONST_INTEGER;
                        lex->parse_lock = true;
                        return NULL;
                    }
                }
                break;
            case 'd':
            case 'D': // decimal format
                lex_next(lex);
                lex_token_clear(&lex->lextkn);
                for (;;) {
                    ch = lex_readc(lex);
                    if ('0' <= ch && ch <= '9') {
                        lex_token_appendc(&lex->lextkn, ch);
                        lex_next(lex);
                    }
                    else {
                        lex->lextkn.token_type = TOKEN_CONST_INTEGER;
                        lex->parse_lock = true;
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
        for (;;) {
            ch = lex_readc(lex);
            if (ch != '"') {
                lex_token_appendc(&lex->lextkn, ch);
                lex_next(lex);
            }
            else {
                lex_next(lex);
                lex->lextkn.token_type = TOKEN_CONST_STRING;
                lex->parse_lock = true;
                return NULL;
            }
        }
    }

    // parsing char constants
    if (ch == '\'') {
        lex_next(lex);
        ch = lex_readc(lex);
        if (ch != '\\') {
            uint8 bytes = utf8_calcu_bytes(ch);
            switch (bytes) {
            case 1:
                lex_token_appendc(&lex->lextkn, ch);
                lex_next(lex);
                break;
            case 2:
                lex_token_appendc(&lex->lextkn, ch);
                lex_next(lex);
                ch = lex_readc(lex);
                lex_token_appendc(&lex->lextkn, ch);
                lex_next(lex);
                break;
            case 3:
                lex_token_appendc(&lex->lextkn, ch);
                lex_next(lex);
                ch = lex_readc(lex);
                lex_token_appendc(&lex->lextkn, ch);
                lex_next(lex);
                ch = lex_readc(lex);
                lex_token_appendc(&lex->lextkn, ch);
                lex_next(lex);
                break;
            case 4:
                lex_token_appendc(&lex->lextkn, ch);
                lex_next(lex);
                ch = lex_readc(lex);
                lex_token_appendc(&lex->lextkn, ch);
                lex_next(lex);
                ch = lex_readc(lex);
                lex_token_appendc(&lex->lextkn, ch);
                lex_next(lex);
                ch = lex_readc(lex);
                lex_token_appendc(&lex->lextkn, ch);
                lex_next(lex);
                break;
            default:
                return new_error("err: invalid UTF-8 code.");
            }
        }
        else {
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
                lex_token_appendc(&lex->lextkn, ch);
                lex_next(lex);
                break;
            default:
                return new_error("err: unknown escape.");
            }
        }
        ch = lex_readc(lex);
        if (ch == '\'') {
            lex->lextkn.token_type = TOKEN_CONST_CHAR;
            lex->parse_lock = true;
            return NULL;
        }
        else {
            return new_error("err: need a single quotation to close the character literal.");
        }
    }

    // parsing operators
    lex_token_appendc(&lex->lextkn, ch);
    lex_next(lex);
    if (ch < 42) {
        switch (ch) {
        case '(':
            lex->lextkn.token_type = TOKEN_OP_LPARENTHESE;
            lex->parse_lock = true;
            return NULL;
        case ')':
            lex->lextkn.token_type = TOKEN_OP_RPARENTHESE;
            lex->parse_lock = true;
            return NULL;
        case '&':
            ch = lex_readc(lex);
            if (ch == '&') {
                lex_token_appendc(&lex->lextkn, '&');
                lex_next(lex);
                lex->lextkn.token_type = TOKEN_OP_LOGIC_AND;
                lex->parse_lock = true;
                return NULL;
            }
            else if (ch == '=') {
                lex_token_appendc(&lex->lextkn, '=');
                lex_next(lex);
                lex->lextkn.token_type = TOKEN_OP_ANDASSIGN;
                lex->parse_lock = true;
                return NULL;
            }
            else {
                lex->lextkn.token_type = TOKEN_OP_AND;
                lex->parse_lock = true;
                return NULL;
            }
        case '!':
            if (lex_readc(lex) == '=') {
                lex_token_appendc(&lex->lextkn, '=');
                lex_next(lex);
                lex->lextkn.token_type = TOKEN_OP_NOTEQ;
                lex->parse_lock = true;
                return NULL;
            }
            else {
                lex->lextkn.token_type = TOKEN_OP_NOT;
                lex->parse_lock = true;
                return NULL;
            }
        case '%':
            if (lex_readc(lex) == '=') {
                lex_token_appendc(&lex->lextkn, '=');
                lex_next(lex);
                lex->lextkn.token_type = TOKEN_OP_MODASSIGN;
                lex->parse_lock = true;
                return NULL;
            }
            else {
                lex->lextkn.token_type = TOKEN_OP_MOD;
                lex->parse_lock = true;
                return NULL;
            }
        }
    }
    else if (ch < 47) {
        switch (ch) {
        case '.':
            lex->lextkn.token_type = TOKEN_OP_SPOT;
            lex->parse_lock = true;
            return NULL;
        case ',':
            lex->lextkn.token_type = TOKEN_OP_COMMA;
            lex->parse_lock = true;
            return NULL;
        case '+':
            ch = lex_readc(lex);
            if (ch == '+') {
                lex_token_appendc(&lex->lextkn, '+');
                lex_next(lex);
                lex->lextkn.token_type = TOKEN_OP_INC;
                lex->parse_lock = true;
                return NULL;
            }
            else if (ch == '=') {
                lex_token_appendc(&lex->lextkn, '=');
                lex_next(lex);
                lex->lextkn.token_type = TOKEN_OP_ADDASSIGN;
                lex->parse_lock = true;
                return NULL;
            }
            else {
                lex->lextkn.token_type = TOKEN_OP_ADD;
                lex->parse_lock = true;
                return NULL;
            }
        case '-':
            ch = lex_readc(lex);
            if (ch == '-') {
                lex_token_appendc(&lex->lextkn, '-');
                lex_next(lex);
                lex->lextkn.token_type = TOKEN_OP_DEC;
                lex->parse_lock = true;
                return NULL;
            }
            else if (ch == '=') {
                lex_token_appendc(&lex->lextkn, '=');
                lex_next(lex);
                lex->lextkn.token_type = TOKEN_OP_SUBASSIGN;
                lex->parse_lock = true;
                return NULL;
            }
            else {
                lex->lextkn.token_type = TOKEN_OP_SUB;
                lex->parse_lock = true;
                return NULL;
            }
        case '*':
            if (lex_readc(lex) == '=') {
                lex_token_appendc(&lex->lextkn, '=');
                lex_next(lex);
                lex->lextkn.token_type = TOKEN_OP_MULASSIGN;
                lex->parse_lock = true;
                return NULL;
            }
            else {
                lex->lextkn.token_type = TOKEN_OP_MUL;
                lex->parse_lock = true;
                return NULL;
            }
        }
    }
    else if (ch < 63) {
        switch (ch) {
        case '=':
            if (lex_readc(lex) == '=') {
                lex_token_appendc(&lex->lextkn, '=');
                lex_next(lex);
                lex->lextkn.token_type = TOKEN_OP_EQ;
                lex->parse_lock = true;
                return NULL;
            }
            else {
                lex->lextkn.token_type = TOKEN_OP_ASSIGN;
                lex->parse_lock = true;
                return NULL;
            }
        case '<':
            ch = lex_readc(lex);
            if (ch == '=') {
                lex_token_appendc(&lex->lextkn, '=');
                lex_next(lex);
                lex->lextkn.token_type = TOKEN_OP_LE;
                lex->parse_lock = true;
                return NULL;
            }
            else if (ch == '<') {
                lex_token_appendc(&lex->lextkn, '<');
                lex_next(lex);
                lex->lextkn.token_type = TOKEN_OP_SHL;
                lex->parse_lock = true;
                return NULL;
            }
            else {
                lex->lextkn.token_type = TOKEN_OP_LT;
                lex->parse_lock = true;
                return NULL;
            }
        case '>':
            ch = lex_readc(lex);
            if (ch == '=') {
                lex_token_appendc(&lex->lextkn, '=');
                lex_next(lex);
                lex->lextkn.token_type = TOKEN_OP_GE;
                lex->parse_lock = true;
                return NULL;
            }
            else if (ch == '>') {
                lex_token_appendc(&lex->lextkn, '>');
                lex_next(lex);
                lex->lextkn.token_type = TOKEN_OP_SHR;
                lex->parse_lock = true;
                return NULL;
            }
            else {
                lex->lextkn.token_type = TOKEN_OP_GT;
                lex->parse_lock = true;
                return NULL;
            }
        case ':':
            lex->lextkn.token_type = TOKEN_OP_COLON;
            lex->parse_lock = true;
            return NULL;
        case '/':
            ch = lex_readc(lex);
            if (ch == '/') {
                // parsing the single line comment
                lex_token_clear(&lex->lextkn);
                lex_next(lex);
                for (;;) {
                    ch = lex_readc(lex);
                    if (ch != '\r' && ch != '\n') {
                        lex_token_appendc(&lex->lextkn, ch);
                        lex_next(lex);
                    }
                    else {
                        lex->lextkn.token_type = TOKEN_OP_SINGLE_CMT;
                        lex->parse_lock = true;
                        return NULL;
                    }
                }
            }
            else if (ch == '*') {
                // parsing the multiple line comment
                lex_token_clear(&lex->lextkn);
                lex_next(lex);
                int embed = 0;
                for (;;) {
                    ch = lex_readc(lex);
                    if (ch == '*') {
                        lex_next(lex);
                        if (lex_readc(lex) == '/') {
                            if (embed <= 0) {
                                lex->lextkn.token_type = TOKEN_OP_MULTIL_CMT;
                                lex->parse_lock = true;
                                return NULL;
                            }
                            else {
                                embed--;
                            }
                        }
                    }
                    else if (ch == '/') {
                        lex_next(lex);
                        if (lex_readc(lex) == '*') {
                            embed++;
                            lex_next(lex);
                        }
                    }
                }
            }
            else if (ch == '=') {
                lex_token_appendc(&lex->lextkn, '=');
                lex_next(lex);
                lex->lextkn.token_type = TOKEN_OP_DIVASSIGN;
                lex->parse_lock = true;
                return NULL;
            }
            else {
                lex->lextkn.token_type = TOKEN_OP_DIV;
                lex->parse_lock = true;
                return NULL;
            }
        }
    }
    else if (ch < 124) {
        switch (ch) {
        case '{':
            lex->lextkn.token_type = TOKEN_OP_LBRACE;
            lex->parse_lock = true;
            return NULL;
        case '[':
            lex->lextkn.token_type = TOKEN_OP_LBRACKET;
            lex->parse_lock = true;
            return NULL;
        case ']':
            lex->lextkn.token_type = TOKEN_OP_RBRACKET;
            lex->parse_lock = true;
            return NULL;
        case '?':
            lex->lextkn.token_type = TOKEN_OP_QUES;
            lex->parse_lock = true;
            return NULL;
        case '^':
            if (lex_readc(lex) == '=') {
                lex_token_appendc(&lex->lextkn, '=');
                lex_next(lex);
                lex->lextkn.token_type = TOKEN_OP_XORASSIGN;
                lex->parse_lock = true;
                return NULL;
            }
            else {
                lex->lextkn.token_type = TOKEN_OP_XOR;
                lex->parse_lock = true;
                return NULL;
            }
        }
    }
    else {
        switch (ch) {
        case '}':
            lex->lextkn.token_type = TOKEN_OP_RBRACE;
            lex->parse_lock = true;
            return NULL;
        case '|':
            ch = lex_readc(lex);
            if (ch == '|') {
                lex_token_appendc(&lex->lextkn, '|');
                lex_next(lex);
                lex->lextkn.token_type = TOKEN_OP_LOGIC_OR;
                lex->parse_lock = true;
                return NULL;
            }
            else if (ch == '=') {
                lex_token_appendc(&lex->lextkn, '=');
                lex_next(lex);
                lex->lextkn.token_type = TOKEN_OP_ORASSIGN;
                lex->parse_lock = true;
                return NULL;
            }
            else {
                lex->lextkn.token_type = TOKEN_OP_OR;
                lex->parse_lock = true;
                return NULL;
            }
        case '~':
            lex->lextkn.token_type = TOKEN_OP_SUB;
            lex->parse_lock = true;
            return NULL;
        }
    }

    lex->lextkn.token_type = TOKEN_UNKNOWN;
    return new_error("err: unknown token");
}

lex_token* lex_read_token(lex_analyzer* lex) {
    return &lex->lextkn;
}

// continue to parse the next token
void lex_next_token(lex_analyzer* lex) {
    lex->parse_lock = false;
}

void lex_destroy(lex_analyzer* lex) {
    lex_token_destroy(&lex->lextkn);
    lex_close_srcfile(lex);
}

#undef lex_next
