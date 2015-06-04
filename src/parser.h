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
#include "closectr.h"
#include "lexer.h"
#include "ident.h"
#include "scope.h"
#include "fileset.h"
#include "ast.h"
#include "expression.h"

#define SYNTAX_ERROR_DEPENDENCE_NEEDED -1

// the parser is used to parse the source code based on
// the rules of C+ programming language syntax.
typedef struct Parser {
    FileStack    file_wait_compiled; // files wait to be compiled
    FileTree     file_have_compiled; // files have been compiled
    Lexer*       lexer;              // the lexer now using
    AST*         ast;                // the abstract syntax tree now building
    LexToken*    cur_token;          // the current token parsed
    Scope*       cur_scope;          // the current scope being parsing
    CloseCounter clsctr;             // guarantee that all brackets are closed correctly
    int8         err_count;          // the number of errors founded until the current stage
}Parser;

extern void  parserInit     (Parser* parser);
extern error parserBuildAST (Parser* parser, char* main_file);
extern void  parserReportErr(Parser* parser, char* errmsg);
extern void  parserDestroy  (Parser* parser);

// the list below is the statement parse functions of the parser. the return values of
// them are uniform:
// (1) NULL -> the lexer is encountering the EOF and the parser can be normally terminated.
// (2)   -1 -> the lexer is encountering some exceptional error.
// (3)   >0 -> all is well
static ASTNodeInclude*     parserParseInclude    (Parser* parser);
static ASTNodeModule*      parserParseModule     (Parser* parser);
static ASTNodeBlock*       parserParseBlock      (Parser* parser);
static ASTNodeID*          parserParseID         (Parser* parser);
static ASTNodeConstLit*    parserParseConstLit   (Parser* parser);
static ASTNodeExpr*        parserParseExpr       (Parser* parser);
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

/*
static void  syntax_analyzer_report_error       (syntax_analyzer* syx, char* errmsg);
static error syntax_analyzer_parse_include      (syntax_analyzer* syx);
static error syntax_analyzer_parse_module       (syntax_analyzer* syx);
static error syntax_analyzer_parse_block        (syntax_analyzer* syx);
static error syntax_analyzer_parse_expr         (syntax_analyzer* syx, smt_expr* expr, bool lhs);
static error syntax_analyzer_parse_expr_list    (syntax_analyzer* syx, smt_expr_list* exprlst);
static error syntax_analyzer_parse_index        (syntax_analyzer* syx, smt_index* idx);
static error syntax_analyzer_parse_decl         (syntax_analyzer* syx, smt_expr* decl_type);
static error syntax_analyzer_parse_assign       (syntax_analyzer* syx, smt_expr* expr_lhs);
static error syntax_analyzer_parse_assigns      (syntax_analyzer* syx, smt_expr_list* exprs_lhs);
static error syntax_analyzer_parse_if           (syntax_analyzer* syx, smt_if* _if);
static error syntax_analyzer_parse_ef           (syntax_analyzer* syx, smt_ef* _ef);
static error syntax_analyzer_parse_else         (syntax_analyzer* syx, smt_else* _else);
static error syntax_analyzer_parse_branch_if    (syntax_analyzer* syx);
static error syntax_analyzer_parse_branch_switch(syntax_analyzer* syx);
static error syntax_analyzer_parse_loop         (syntax_analyzer* syx);
static error syntax_analyzer_parse_loop_for     (syntax_analyzer* syx);
static error syntax_analyzer_parse_loop_while   (syntax_analyzer* syx);
static error syntax_analyzer_parse_loop_foreach (syntax_analyzer* syx);
static error syntax_analyzer_parse_func_def     (syntax_analyzer* syx);
static error syntax_analyzer_parse_func_call    (syntax_analyzer* syx, smt_func_call* call);
static error syntax_analyzer_parse_return       (syntax_analyzer* syx);
static error syntax_analyzer_parse_type         (syntax_analyzer* syx);
static error syntax_analyzer_parse_type_decl    (syntax_analyzer* syx);
static error syntax_analyzer_parse_type_assign  (syntax_analyzer* syx);
static error syntax_analyzer_parse_type_def     (syntax_analyzer* syx);
static error syntax_analyzer_parse_new          (syntax_analyzer* syx);
static error syntax_analyzer_parse_error        (syntax_analyzer* syx);
static error syntax_analyzer_parse_deal         (syntax_analyzer* syx);
*/

#endif
