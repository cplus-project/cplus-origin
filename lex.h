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

#define TOKEN_UNKNOWN          0x000  // all unknown token type
#define TOKEN_ID               0x100  // identifiers
#define TOKEN_KEYWORD_IF       0x200  // if
#define TOKEN_KEYWORD_EF       0x201  // ef
#define TOKEN_KEYWORD_ELSE     0x202  // else
#define TOKEN_KEYWORD_FOR      0x203  // for
#define TOKEN_KEYWORD_BREAK    0x204  // break
#define TOKEN_KEYWORD_CONTINUE 0x205  // continue
#define TOKEN_KEYWORD_FUNC     0x206  // func
#define TOKEN_KEYWORD_RETURN   0x207  // return
#define TOKEN_KEYWORD_ERROR    0x208  // error
#define TOKEN_KEYWORD_DEAL     0x209  // deal
#define TOKEN_KEYWORD_CASE     0x20A  // case
#define TOKEN_KEYWORD_TYPE     0x20B  // type
#define TOKEN_KEYWORD_IN       0x20C  // in
#define TOKEN_KEYWORD_OT       0x20D  // ot
#define TOKEN_CONST_NUMBER     0x300  // const-number
#define TOKEN_CONST_CHAR       0x301  // const-char
#define TOKEN_CONST_STRING     0x302  // const-string
#define TOKEN_OP_ASSIGN        0x400  // =
#define TOKEN_OP_ADD           0X401  // +
#define TOKEN_OP_SUB           0x402  // -
#define TOKEN_OP_MUL           0x403  // *
#define TOKEN_OP_AND           0x404  // &
#define TOKEN_OP_OR            0x405  // |
#define TOKEN_OP_XOR           0x406  // ^
#define TOKEN_OP_DIV           0x407  // /
#define TOKEN_OP_NEG           0x408  // ~
#define TOKEN_OP_SINGLE_QUO    0x409  // '
#define TOKEN_OP_DOUBLE_QUO    0x40A  // "
#define TOKEN_OP_LPARENTHESE   0x40B  // (
#define TOKEN_OP_LBRACKET      0x40C  // [
#define TOKEN_OP_LBRACE        0x40D  // {
#define TOKEN_OP_RPARENTHESE   0X40E  // )
#define TOKEN_OP_RBRACKET      0x40F  // ]
#define TOKEN_OP_RBRACE        0x410  // }
#define TOKEN_OP_SPOT          0x411  // .
#define TOKEN_OP_SEMC          0x412  // ;
#define TOKEN_OP_LT            0x413  // <
#define TOKEN_OP_GT            0x414  // >
#define TOKEN_OP_INC           0x415  // ++
#define TOKEN_OP_DEC           0x416  // --
#define TOKEN_OP_SHL           0X417  // <<
#define TOKEN_OP_SHR           0x418  // >>
#define TOKEN_OP_EQ            0x419  // ==
#define TOKEN_OP_LE            0x41A  // <=
#define TOKEN_OP_GE            0x41B  // >=
#define TOKEN_OP_ADDASSIGN     0x41C  // +=
#define TOKEN_OP_SUBASSIGN     0x41D  // -=
#define TOKEN_OP_MULASSIGN     0X41E  // *=
#define TOKEN_OP_DIVASSIGN     0x41F  // /=
#define TOKEN_OP_ANDASSIGN     0X420  // &=
#define TOKEN_OP_ORASSIGN      0x421  // |=
#define TOKEN_OP_XORASSIGN     0x422  // ^=
#define TOKEN_OP_LOGIC_AND     0x423  // &&
#define TOKEN_OP_LOGIC_OR      0x424  // ||
#define TOKEN_OP_ONELINE_CMT   0x425  // //
#define TOKEN_OP_LMULTIL_CMT   0x426  // /*
#define TOKEN_OP_RMULTIL_CMT   0x427  // */

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
