/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     The lexer.h and lexer.c define and implement the lexical
 * analyzer of the C+ programming language.
 **/

#ifndef CPLUS_LEXER_H
#define CPLUS_LEXER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "dynamicarr.h"
#include "convert.h"
#include "utf.h"

#define TOKEN_UNKNOWN          000  // all unknown token type
#define TOKEN_ID               100  // identifier
#define TOKEN_KEYWORD_CONST    200  // const
#define TOKEN_KEYWORD_IF       201  // if
#define TOKEN_KEYWORD_EF       202  // ef
#define TOKEN_KEYWORD_ELSE     203  // else
#define TOKEN_KEYWORD_FOR      204  // for
#define TOKEN_KEYWORD_BREAK    205  // break
#define TOKEN_KEYWORD_CONTINUE 206  // continue
#define TOKEN_KEYWORD_FTHROUGH 207  // fallthrough
#define TOKEN_KEYWORD_FUNC     208  // func
#define TOKEN_KEYWORD_EXPN     209  // expn
#define TOKEN_KEYWORD_RETURN   210  // return
#define TOKEN_KEYWORD_ERROR    211  // error
#define TOKEN_KEYWORD_SWITCH   212  // switch
#define TOKEN_KEYWORD_DEAL     213  // deal
#define TOKEN_KEYWORD_CASE     214  // case
#define TOKEN_KEYWORD_DEFAULT  215  // default
#define TOKEN_KEYWORD_TYPE     216  // type
#define TOKEN_KEYWORD_NEW      217  // new
#define TOKEN_KEYWORD_INCLUDE  218  // include
#define TOKEN_KEYWORD_MODULE   219  // module
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
#define TOKEN_LINEFEED         502  // line-feed
#define TOKEN_TYPE_BYTE        600  // byte
#define TOKEN_TYPE_INT8        601  // int8
#define TOKEN_TYPE_INT16       602  // int16
#define TOKEN_TYPE_INT32       603  // int32
#define TOKEN_TYPE_INT64       604  // int64
#define TOKEN_TYPE_UINT        605  // uint
#define TOKEN_TYPE_UINT8       606  // uint8
#define TOKEN_TYPE_UINT16      607  // uint16
#define TOKEN_TYPE_UINT32      608  // uint32
#define TOKEN_TYPE_UINT64      609  // uint64
#define TOKEN_TYPE_FLOAT32     610  // float32
#define TOKEN_TYPE_FLOAT64     611  // float64
#define TOKEN_TYPE_COMPLEX64   612  // complex64
#define TOKEN_TYPE_COMPLEX128  613  // complex128
#define TOKEN_TYPE_CHAR        614  // char
#define TOKEN_TYPE_STRING      615  // string

// the micro definitions listd below can be used to confirm one lex_token's
// type, you can use them as the example:
//    LexTkn lextkn;
//    ...
//    if (tokenIsID(lextkn.token_type) || tokenIsKeyword(lextkn.token_type)) {
//        // process the token here
//    }
//    ...
//
#define tokenIsID(token_code)       (token_code == 100)
#define tokenIsKeywords(token_code) (200 <= token_code && token_code < 300)
#define tokenIsConstLit(token_code) (300 <= token_code && token_code < 400)
#define tokenIsOperator(token_code) (400 <= token_code && token_code < 500)
#define tokenIsPrimType(token_code) (600 <= token_code && token_code < 700)

#define LEX_ERROR_EOF         -1

#define EXTRA_INFO_OP_LUNARY   1
#define EXTRA_INFO_OP_RUNARY   2
#define EXTRA_INFO_OP_BINARY   3
#define EXTRA_INFO_EXPR_END    4
#define EXTRA_INFO_ASSIGN      5
typedef struct {
    DynamicArrChar token;      // one dynamic char array to store the token's content
    int64          token_len;  // save the token's length
    int16          token_code; // will be assigned with one of micro definitions prefixed with 'TOKEN_...'
    int8           extra_info; // extra information of the token
}LexToken;

extern error lexTokenInit   (LexToken* lextkn, int64 capacity);
extern void  lexTokenAppend (LexToken* lextkn, char* str, int64 len);
extern void  lexTokenAppendc(LexToken* lextkn, char  ch);
extern char* lexTokenGetStr (LexToken* lextkn);
extern void  lexTokenClear  (LexToken* lextkn);
extern void  lexTokenDebug  (LexToken* lextkn);
extern void  lexTokenDestroy(LexToken* lextkn);

// if want to change the lexical analyzer's buffer size and file
// read rate, just modify the under micro definition.
//
// node:
//   the LEX_BUFF_SIZE should not exceed 32767(is 2^16/2-1), because
//   the index 'i' and 'buff_end_index' are 16bits numbers.
//
#define LEX_BUFF_SIZE 4096
typedef struct{
    FILE*    srcfile;               // source file descriptor
    char     buffer[LEX_BUFF_SIZE]; // the buffer used to read the source file
    int16    i;                     // the array index of buffer
    int16    buff_end_index;        // the last index of the buffer. the buffer will not be
                                    // always filled with the capacity of LEX_BUFF_SIZE so
                                    // the buff_end_index will flag this situation
    char*    pos_file;              // the source file name now parsing
    int32    pos_line;              // record the current analyzing line count
    int16    pos_col;               // record the position in the current line
    LexToken lextkn;                // to storage the information of the token which is parsing now
    bool     parse_lock;            // if the parse_lock == true, the lexical analyzer can not
                                    // continue to parse the next token
}Lexer;

// first initialize the lexer before using it and then you can use the lexer as
// the work-flow displayed below:
//   ...
//   Lexer     lexer;
//   LexToken* lextkn;
//   ...
//   for (;;) {
//       // parse a token from the source code buffer
//       error err = lexerParseToken(&lexer);
//       if (err != NULL) {
//           return err;
//       }
//       // read the token parsed just now from the buffer
//       lextkn = lexerReadToken(&lexer);
//
//       // do some operations for the token parsed at here...
//
//       // unlock the lexer ready to parse the next token
//       lexerNextToken(&lexer);
//   }
//   ...
//
extern error     lexerInit        (Lexer* lexer);
extern error     lexerOpenSrcFile (Lexer* lexer, char* file);
extern void      lexerCloseSrcFile(Lexer* lexer);
extern error     lexerParseToken  (Lexer* lexer);
extern LexToken* lexerReadToken   (Lexer* lexer);
extern void      lexerNextToken   (Lexer* lexer);
extern void      lexerDestroy     (Lexer* lexer);

#endif
