/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     The lex.h and lex.c define and implement the lexical
 * analyzer of the c+ programming language.
 **/

#ifndef CPLUS_LEX_H
#define CPLUS_LEX_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "dynamicarr.h"
#include "convert.h"
#include "utf.h"

#define TOKEN_UNKNOWN          000  // all unknown token type
#define TOKEN_ID               100  // identifier
#define TOKEN_KEYWORD_IF       200  // if
#define TOKEN_KEYWORD_EF       201  // ef
#define TOKEN_KEYWORD_ELSE     202  // else
#define TOKEN_KEYWORD_FOR      203  // for
#define TOKEN_KEYWORD_BREAK    204  // break
#define TOKEN_KEYWORD_CONTINUE 205  // continue
#define TOKEN_KEYWORD_FUNC     206  // func
#define TOKEN_KEYWORD_RETURN   207  // return
#define TOKEN_KEYWORD_ERROR    208  // error
#define TOKEN_KEYWORD_SWITCH   209  // switch
#define TOKEN_KEYWORD_DEAL     210  // deal
#define TOKEN_KEYWORD_CASE     211  // case
#define TOKEN_KEYWORD_DEFAULT  212  // default
#define TOKEN_KEYWORD_TYPE     213  // type
#define TOKEN_KEYWORD_NEW      214  // new
#define TOKEN_KEYWORD_IN       215  // in
#define TOKEN_KEYWORD_OT       216  // ot
#define TOKEN_KEYWORD_INCLUDE  217  // include
#define TOKEN_KEYWORD_MODULE   218  // module
#define TOKEN_CONST_INTEGER    300  // const-integer
#define TOKEN_CONST_FLOAT      301  // const-float
#define TOKEN_CONST_CHAR       302  // const-char
#define TOKEN_CONST_STRING     303  // const-string
#define TOKEN_OP_ASSIGN        400  // =
#define TOKEN_OP_ADDASSIGN     401  // +=
#define TOKEN_OP_SUBASSIGN     402  // -=
#define TOKEN_OP_MULASSIGN     403  // *=
#define TOKEN_OP_DIVASSIGN     404  // /=
#define TOKEN_OP_MODASSIGN     405  // %=
#define TOKEN_OP_ANDASSIGN     406  // &=
#define TOKEN_OP_ORASSIGN      407  // |=
#define TOKEN_OP_XORASSIGN     408  // ^=
#define TOKEN_OP_SPOT          409  // .
#define TOKEN_OP_LPARENTHESE   410  // (
#define TOKEN_OP_RPARENTHESE   411  // )
#define TOKEN_OP_LBRACE        412  // {
#define TOKEN_OP_RBRACE        413  // }
#define TOKEN_OP_INC           414  // ++
#define TOKEN_OP_DEC           415  // --
#define TOKEN_OP_NEG           416  // -
#define TOKEN_OP_DEREFER       417  // $
#define TOKEN_OP_GETADDR       418  // @
#define TOKEN_OP_LBRACKET      419  // [
#define TOKEN_OP_RBRACKET      420  // ]
#define TOKEN_OP_MUL           421  // *
#define TOKEN_OP_DIV           422  // /
#define TOKEN_OP_MOD           423  // %
#define TOKEN_OP_ADD           424  // +
#define TOKEN_OP_SUB           425  // -
#define TOKEN_OP_SHL           426  // <<
#define TOKEN_OP_SHR           427  // >>
#define TOKEN_OP_AND           428  // &
#define TOKEN_OP_OR            429  // |
#define TOKEN_OP_XOR           430  // ^
#define TOKEN_OP_NOT           431  // !
#define TOKEN_OP_EQ            432  // ==
#define TOKEN_OP_NOTEQ         433  // !=
#define TOKEN_OP_LT            434  // <
#define TOKEN_OP_GT            435  // >
#define TOKEN_OP_LE            436  // <=
#define TOKEN_OP_GE            437  // >=
#define TOKEN_OP_LOGIC_AND     438  // &&
#define TOKEN_OP_LOGIC_OR      439  // ||
#define TOKEN_OP_SINGLE_QUO    440  // '
#define TOKEN_OP_DOUBLE_QUO    441  // "
#define TOKEN_OP_COMMA         442  // ,
#define TOKEN_OP_SEMC          443  // ;
#define TOKEN_OP_QUES          444  // ?
#define TOKEN_OP_COLON         445  // :
#define TOKEN_OP_SINGLE_CMT    446  // //
#define TOKEN_OP_MULTIL_CMT    447  // /**/
#define TOKEN_NEXT_LINE        502  // change-line symbol

// the micro definitions listd below can be used to confirm one lex_token's
// type, you can use them as the example:
//    lex_token lextkn;
//    ...
//    if (token_isid(lextkn.token_type) || token_iskeyword(lextkn.token_type)) {
//        // process the token here
//    }
//    ...
#define token_isid(token_code)       (token_code == 100)
#define token_iskeyword(token_code)  (200 <= token_code && token_code < 300)
#define token_isconstlit(token_code) (300 <= token_code && token_code < 400)
#define token_isoperator(token_code) (400 <= token_code && token_code < 500)

#define LEX_ERROR_EOF          -1

#define EXTRA_INFO_OP_LUNARY   1
#define EXTRA_INFO_OP_RUNARY   2
#define EXTRA_INFO_OP_BINARY   3
typedef struct {
    dynamicarr_char token;      // one dynamic char array to store the token's content
    int64           token_len;  // token's length
    int16           token_type; // will be assigned with one of micro definitions prefixed with 'TOKEN_...'
    int8            extra_info; // extra information of the token
}lex_token;

extern error lex_token_init   (lex_token* lextkn, int64 capacity);
extern void  lex_token_append (lex_token* lextkn, char* str, int64 len);
extern void  lex_token_appendc(lex_token* lextkn, char  ch);
extern char* lex_token_getstr (lex_token* lextkn);
extern void  lex_token_clear  (lex_token* lextkn);
extern void  lex_token_debug  (lex_token* lextkn);
extern void  lex_token_destroy(lex_token* lextkn);

typedef struct lex_token_stack_node {
    char* token;
    int16 token_type;
    struct lex_token_stack_node* next;
}lex_token_stack_node;

// the lex_token_stack is a type of stack used to save a set of
// lexical tokens.
typedef struct {
    lex_token_stack_node* top;
}lex_token_stack;

extern void  lex_token_stack_init     (lex_token_stack* lexstk);
extern void  lex_token_stack_push     (lex_token_stack* lexstk, char* token, int16 token_type);
extern bool  lex_token_stack_isempty  (lex_token_stack* lexstk);
extern char* lex_token_stack_top_token(lex_token_stack* lexstk);
extern int16 lex_token_stack_top_type (lex_token_stack* lexstk);
extern void  lex_token_stack_pop      (lex_token_stack* lexstk);
extern void  lex_token_stack_destroy  (lex_token_stack* lexstk);

// if want to change the lexical analyzer's buffer size and file
// read rate, just modify the under micro definition.
// node:
//   the LEX_BUFF_SIZE should not exceed 32767(is 2^16/2-1), because
//   the index 'i' and 'buff_end_index' are 16bits numbers.
#define LEX_BUFF_SIZE 4096
typedef struct{
    FILE* srcfile;
    char  buffer[LEX_BUFF_SIZE];
    int16 i;              // the array index of buffer
    int16 buff_end_index; // the last index of the buffer. the buffer will not be
                          // always filled with the capacity of LEX_BUFF_SIZE so
                          // the buff_end_index will flag this situation

    int32     line;       // record the current analyzing line count
    int16     col;        // record the position in the current line
    lex_token lextkn;     // to storage the information of the token which is parsing now
    bool      parse_lock; // if the parse_lock == true, the lexical analyzer can not
                          // continue to parse the next token
}lex_analyzer;

// first initialize the lexical analyzer before using it.
// and then you can use the lexical analyzer as the work-flow
// displayed below:
//   ...
//   lex_analyzer lex;
//   lex_token*   lextkn;
//   ...
//   for (;;) {
//       // parse a token from the source code buffer
//       error err = lex_parse_token(&lex);
//       if (err != NULL) {
//           return err;
//       }
//       // read the token parsed just now from the buffer
//       lextkn = lex_read_token(&lex);
//
//       // do some operations for the token parsed at here...
//
//       // clear the current token in the buffer and ready to
//       // parse the next token
//       lex_next_token(&lex);
//   }
//   ...
extern error      lex_init         (lex_analyzer* lex);
extern error      lex_open_srcfile (lex_analyzer* lex, char* file);
extern void       lex_close_srcfile(lex_analyzer* lex);
extern error      lex_parse_token  (lex_analyzer* lex);
extern lex_token* lex_read_token   (lex_analyzer* lex);
extern void       lex_next_token   (lex_analyzer* lex);
extern void       lex_destroy      (lex_analyzer* lex);

#endif
