/**
 * Copyright by JiKai in 2014-12-10.
 *
 *     The lex.h and lex.c define and implement the lexical
 * analyzer of the c+ programming language.
 **/

#ifndef CPLUS_LEX_H
#define CPLUS_LEX_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "common.h"
#include "dynamicarr.h"

#define TOKEN_UNKNOWN          0x000  // all unknown token type
#define TOKEN_KEYWORD_IF       0x101  // if
#define TOKEN_KEYWORD_EF       0x102  // ef
#define TOKEN_KEYWORD_ELSE     0x103  // else
#define TOKEN_KEYWORD_FOR      0x104  // for
#define TOKEN_KEYWORD_BREAK    0x105  // break
#define TOKEN_KEYWORD_CONTINUE 0x106  // continue
#define TOKEN_KEYWORD_FUNC     0x107  // func
#define TOKEN_KEYWORD_RETURN   0x108  // return
#define TOKEN_KEYWORD_ERROR    0x109  // error
#define TOKEN_KEYWORD_DEAL     0x110  // deal
#define TOKEN_KEYWORD_CASE     0x111  // case
#define TOKEN_KEYWORD_TYPE     0x112  // type
#define TOKEN_CONST_INTEGER    0x200  // const-integer
#define TOKEN_CONST_FLOAT      0x201  // const-float
#define TOKEN_CONST_STRING     0x202  // const-string
#define TOKEN_OP_ASSIGN        0x300  // =
#define TOKEN_OP_ADD           0X301  // +
#define TOKEN_OP_SUB           0x302  // -
#define TOKEN_OP_MUL           0x303  // *
#define TOKEN_OP_AND           0x304  // &
#define TOKEN_OP_OR            0x305  // |
#define TOKEN_OP_XOR           0x306  // ^
#define TOKEN_OP_DIV           0x307  // /
#define TOKEN_OP_NEG           0x308  // ~
#define TOKEN_OP_SINGLE_QUO    0x309  // '
#define TOKEN_OP_DOUBLE_QUO    0x30A  // "
#define TOKEN_OP_LPARENTHESE   0x30B  // (
#define TOKEN_OP_LBRACKET      0x30C  // [
#define TOKEN_OP_LBRACE        0x30D  // {
#define TOKEN_OP_RPARENTHESE   0X30E  // )
#define TOKEN_OP_RBRACKET      0x30F  // ]
#define TOKEN_OP_RBRACE        0x310  // }
#define TOKEN_OP_SPOT          0x311  // .
#define TOKEN_OP_SEMC          0x312  // ;
#define TOKEN_OP_LT            0x313  // <
#define TOKEN_OP_GT            0x314  // >
#define TOKEN_OP_INC           0x315  // ++
#define TOKEN_OP_DEC           0x316  // --
#define TOKEN_OP_SHL           0X317  // <<
#define TOKEN_OP_SHR           0x318  // >>
#define TOKEN_OP_EQ            0x319  // ==
#define TOKEN_OP_LE            0x31A  // <=
#define TOKEN_OP_GE            0x31B  // >=
#define TOKEN_OP_ADDASSIGN     0x31C  // +=
#define TOKEN_OP_SUBASSIGN     0x31D  // -=
#define TOKEN_OP_MULASSIGN     0X31E  // *=
#define TOKEN_OP_DIVASSIGN     0x31F  // /=
#define TOKEN_OP_ANDASSIGN     0X320  // &=
#define TOKEN_OP_ORASSIGN      0x321  // |=
#define TOKEN_OP_XORASSIGN     0x322  // ^=
#define TOKEN_OP_LOGIC_AND     0x323  // &&
#define TOKEN_OP_LOGIC_OR      0x324  // ||
#define TOKEN_OP_ONELINE_CMT   0x325  // //
#define TOKEN_OP_LMULTIL_CMT   0x326  // /*
#define TOKEN_OP_RMULTIL_CMT   0x327  // */

typedef struct {
    dynamicarr_char token; // one dynamic char array to store the token's content
    int32 token_len;       // token's length
    int32 token_type;      // will be assigned with one of micro definitions suffixed with 'TOKEN_...'
}lex_token;

extern error lex_token_init   (lex_token* lextkn, uint64 capacity);
extern void  lex_token_destroy(lex_token* lextkn);

// if want to change the lexical analyzer's buffer size and file
// read rate, just modify the under micro definition.
#define LEX_BUFF_SIZE 4096
typedef struct{
    FILE* srcfile;
    char  buffer[LEX_BUFF_SIZE];
    int32 i;    // the array index of buffer
    int32 buff_end_index; // the last index of the buffer. the buffer will not be
                          // always filled with the capacity of LEX_BUFF_SIZE so
                          // the buff_end_index will flag this situation
    int32 line; // record the current analyzing line count
}lex_analyzer;

// first initialize the lexical analyzer before using it.
extern void   lex_init         (lex_analyzer* lex);
extern error  lex_open_srcfile (lex_analyzer* lex, char* file);
extern void   lex_close_srcfile(lex_analyzer* lex);
extern error  lex_parse_token  (lex_analyzer* lex, lex_token* lextkn);
extern error  lex_peek_token   (lex_analyzer* lex, lex_token* lextkn);

#endif
