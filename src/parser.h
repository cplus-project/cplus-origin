/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     The parser.h and parser.c implement the parser of
 * C+ language. The output of the parser is an AST(which
 * implemented in ast.h and ast.c).
 **/

#ifndef CPLUS_PARSER_H
#define CPLUS_PARSER_H

#include "common.h"
#include "lexer.h"
#include "ident.h"
#include "fileset.h"
#include "ast.h"
#include "expression.h"
#include "scope.h"

// the parser is used to parse the source code based on
// the rules of C+ programming language syntax.
typedef struct Parser {
    FileStack    file_wait_compiled; // files wait to be compiled
    FileTree     file_have_compiled; // files have been compiled
    Lexer*       lexer;              // the lexer now using
    AST*         ast;                // the abstract syntax tree now building
    LexToken*    cur_token;          // the current token parsed
    Scope*       cur_scope;          // the current scope being parsed
    int8         err_count;          // the number of errors founded until the current stage
}Parser;

extern void  parserInit     (Parser* parser);
extern error parserStart    (Parser* parser, char* main_file);
extern void  parserReportErr(Parser* parser, char* errmsg);
extern void  parserDestroy  (Parser* parser);

static ASTNodeInclude*     parserParseInclude    (Parser* parser);
static ASTNodeModule*      parserParseModule     (Parser* parser);
static ASTNodeBlock*       parserParseBlock      (Parser* parser);
static ASTNodeCaseBody*    parserParseCaseBody   (Parser* parser);
static ASTNodeGlobalScope* parserParseGlobalScope(Parser* parser);
static ASTNodeConstLit*    parserParseConstLit   (Parser* parser);
static ASTNodeID*          parserParseID         (Parser* parser);
static ASTNodeExpr*        parserParseExpr       (Parser* parser);
static ASTNodeExprList*    parserParseExprList   (Parser* parser);
static ASTNodeIndex*       parserParseIndex      (Parser* parser);
static ASTNodeDecl*        parserParseDecl       (Parser* parser);
static ASTNodeAssign*      parserParseAssign     (Parser* parser);
static ASTNodeIf*          parserParseIf         (Parser* parser);
static ASTNodeEf*          parserParseEf         (Parser* parser);
static ASTNodeElse*        parserParseElse       (Parser* parser);
static ASTNodeSwitch*      parserParseSwitch     (Parser* parser);
static ASTNodeSwitchCase*  parserParseSwitchCase (Parser* parser);
static ASTNodeSwitchDeft*  parserParseSwitchDeft (Parser* parser);
static ASTNodeLoopFor*     parserParseLoopFor    (Parser* parser);
static ASTNodeLoopWhile*   parserParseLoopWhile  (Parser* parser);
static ASTNodeLoopInf*     parserParseLoopInf    (Parser* parser);
static ASTNodeLoopForeach* parserParseLoopForeach(Parser* parser);
static ASTNodeFuncDef*     parserParseFuncDef    (Parser* parser);
static ASTNodeFuncCall*    parserParseFuncCall   (Parser* parser);
static ASTNodeReturn*      parserParseReturn     (Parser* parser);
static ASTNodeTypeDecl*    parserParseTypeDecl   (Parser* parser);
static ASTNodeTypeDef*     parserParseTypeDef    (Parser* parser);
static ASTNodeNew*         parserParseNew        (Parser* parser);
static ASTNodeError*       parserParseError      (Parser* parser);
static ASTNodeDeal*        parserParseDeal       (Parser* parser);

#endif
