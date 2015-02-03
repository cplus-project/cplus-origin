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

#define TOKEN_UNKNOWN          000  // all unknown token type
#define TOKEN_ID               100  // identifiers
#define TOKEN_KEYWORD_IF       200  // if
#define TOKEN_KEYWORD_EF       201  // ef
#define TOKEN_KEYWORD_ELSE     202  // else
#define TOKEN_KEYWORD_FOR      203  // for
#define TOKEN_KEYWORD_BREAK    204  // break
#define TOKEN_KEYWORD_CONTINUE 205  // continue
#define TOKEN_KEYWORD_FUNC     206  // func
#define TOKEN_KEYWORD_RETURN   207  // return
#define TOKEN_KEYWORD_ERROR    208  // error
#define TOKEN_KEYWORD_DEAL     209  // deal
#define TOKEN_KEYWORD_CASE     210  // case
#define TOKEN_KEYWORD_TYPE     211  // type
#define TOKEN_KEYWORD_IN       212  // in
#define TOKEN_KEYWORD_OT       213  // ot
#define TOKEN_CONST_NUMBER     300  // const-number
#define TOKEN_CONST_CHAR       301  // const-char
#define TOKEN_CONST_STRING     302  // const-string
#define TOKEN_OP_ASSIGN        400  // =
#define TOKEN_OP_ADD           401  // +
#define TOKEN_OP_SUB           402  // -
#define TOKEN_OP_MUL           403  // *
#define TOKEN_OP_AND           404  // &
#define TOKEN_OP_OR            405  // |
#define TOKEN_OP_XOR           406  // ^
#define TOKEN_OP_DIV           407  // /
#define TOKEN_OP_MOD           408  // %
#define TOKEN_OP_NEG           409  // ~
#define TOKEN_OP_SINGLE_QUO    410  // '
#define TOKEN_OP_DOUBLE_QUO    411  // "
#define TOKEN_OP_LPARENTHESE   412  // (
#define TOKEN_OP_LBRACKET      413  // [
#define TOKEN_OP_LBRACE        414  // {
#define TOKEN_OP_RPARENTHESE   415  // )
#define TOKEN_OP_RBRACKET      416  // ]
#define TOKEN_OP_RBRACE        417  // }
#define TOKEN_OP_SPOT          418  // .
#define TOKEN_OP_COMMA         419  // ,
#define TOKEN_OP_SEMC          420  // ;
#define TOKEN_OP_QUES          421  // ?
#define TOKEN_OP_COLON         422  // :
#define TOKEN_OP_LT            423  // <
#define TOKEN_OP_GT            424  // >
#define TOKEN_OP_NOT           425  // !
#define TOKEN_OP_INC           426  // ++
#define TOKEN_OP_DEC           427  // --
#define TOKEN_OP_SHL           428  // <<
#define TOKEN_OP_SHR           429  // >>
#define TOKEN_OP_EQ            430  // ==
#define TOKEN_OP_NOTEQ         431  // !=
#define TOKEN_OP_LE            432  // <=
#define TOKEN_OP_GE            433  // >=
#define TOKEN_OP_ADDASSIGN     434  // +=
#define TOKEN_OP_SUBASSIGN     435  // -=
#define TOKEN_OP_MULASSIGN     436  // *=
#define TOKEN_OP_DIVASSIGN     437  // /=
#define TOKEN_OP_MODASSIGN     438  // %=
#define TOKEN_OP_ANDASSIGN     439  // &=
#define TOKEN_OP_ORASSIGN      440  // |=
#define TOKEN_OP_XORASSIGN     441  // ^=
#define TOKEN_OP_LOGIC_AND     442  // &&
#define TOKEN_OP_LOGIC_OR      443  // ||
#define TOKEN_OP_ONELINE_CMT   444  // //
#define TOKEN_OP_LMULTIL_CMT   445  // /*
#define TOKEN_OP_RMULTIL_CMT   446  // */

typedef struct {
    dynamicarr_char token;      // one dynamic char array to store the token's content
    uint64          token_len;  // token's length
    int32           token_type; // will be assigned with one of micro definitions prefixed with 'TOKEN_...'
}lex_token;

extern error lex_token_init   (lex_token* lextkn, uint64 capacity);
extern void  lex_token_append (lex_token* lextkn, char* str, uint64 len);
extern void  lex_token_appendc(lex_token* lextkn, char ch);
extern char* lex_token_getstr (lex_token* lextkn);
extern void  lex_token_clear  (lex_token* lextkn);
extern void  lex_token_destroy(lex_token* lextkn);

// if want to change the lexical analyzer's buffer size and file
// read rate, just modify the under micro definition.
#define LEX_BUFF_SIZE 4096
typedef struct{
    FILE* srcfile;
    char  buffer[LEX_BUFF_SIZE];
    int64 i;    // the array index of buffer
    int64 buff_end_index; // the last index of the buffer. the buffer will not be
                          // always filled with the capacity of LEX_BUFF_SIZE so
                          // the buff_end_index will flag this situation
    int64 line; // record the current analyzing line count
}lex_analyzer;

// first initialize the lexical analyzer before using it.
extern void   lex_init         (lex_analyzer* lex);
extern error  lex_open_srcfile (lex_analyzer* lex, char* file);
extern void   lex_close_srcfile(lex_analyzer* lex);
extern error  lex_parse_token  (lex_analyzer* lex, lex_token* lextkn);
extern error  lex_peek_token   (lex_analyzer* lex, lex_token* lextkn);

#endif
