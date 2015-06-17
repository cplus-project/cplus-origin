/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     This file ast.h and ast.c implement the abstract
 * syntax tree of the C+ language.
 **/

#ifndef CPLUS_AST_H
#define CPLUS_AST_H

#include "common.h"
#include "ident.h"

#define AST_NODE                   0x00
#define AST_NODE_STMT              0x01
#define AST_NDOE_INCLUDE           0x02
#define AST_NODE_MODULE            0x03
#define AST_NODE_BLOCK             0x04
#define AST_NODE_CASE_BODY         0x05
#define AST_NODE_GLOBAL_SCOPE      0x06
#define AST_NODE_CONST_LIT         0x07
#define AST_NODE_ID                0x08
#define AST_NODE_EXPR              0x09
#define AST_NODE_EXPR_LIST         0x0A
#define AST_NODE_EXPR_UNRY         0x0B
#define AST_NODE_EXPR_BNRY         0x0C
#define AST_NODE_INDEX             0x0D
#define AST_NODE_DECL              0x0E
#define AST_NODE_ASSIGN            0x0F
#define AST_NODE_IF                0x10
#define AST_NODE_EF                0x11
#define AST_NODE_ELSE              0x12
#define AST_NODE_SWITCH            0x13
#define AST_NODE_SWITCH_CASE       0x14
#define AST_NODE_SWITCH_DEFT       0x15
#define AST_NODE_LOOP_FOR          0x16
#define AST_NODE_LOOP_WHILE        0x17
#define AST_NODE_LOOP_INF          0x18
#define AST_NODE_LOOP_FOREACH      0x19
#define AST_NODE_FUNC_DEF          0x1A
#define AST_NODE_FUNC_CALL         0x1B
#define AST_NODE_RETURN            0x1C
#define AST_NODE_TYPE_DECL         0x1D
#define AST_NODE_TYPE_DEF          0x1E
#define AST_NODE_NEW               0x1F
#define AST_NODE_ERROR             0x20
#define AST_NODE_DEAL              0x21

typedef struct ASTNode            ASTNode;
typedef struct ASTNodeStmt        ASTNodeStmt;
typedef struct ASTNodeInclude     ASTNodeInclude;
typedef struct ASTNodeModule      ASTNodeModule;
typedef struct ASTNodeBlock       ASTNodeBlock;
typedef struct ASTNodeCaseBody    ASTNodeCaseBody;
typedef struct ASTNodeGlobalScope ASTNodeGlobalScope;
typedef struct ASTNodeID          ASTNodeID;
typedef struct ASTNodeConstLit    ASTNodeConstLit;
typedef struct ASTNodeExpr        ASTNodeExpr;
typedef struct ASTNodeExprList    ASTNodeExprList;
typedef struct ASTNodeExprUnry    ASTNodeExprUnry;
typedef struct ASTNodeExprBnry    ASTNodeExprBnry;
typedef struct ASTNodeIndex       ASTNodeIndex;
typedef struct ASTNodeDecl        ASTNodeDecl;
typedef struct ASTNodeAssign      ASTNodeAssign;
typedef struct ASTNodeIf          ASTNodeIf;
typedef struct ASTNodeEf          ASTNodeEf;
typedef struct ASTNodeElse        ASTNodeElse;
typedef struct ASTNodeSwitch      ASTNodeSwitch;
typedef struct ASTNodeSwitchCase  ASTNodeSwitchCase;
typedef struct ASTNodeSwitchDeft  ASTNodeSwitchDeft;
typedef struct ASTNodeLoopFor     ASTNodeLoopFor;
typedef struct ASTNodeLoopWhile   ASTNodeLoopWhile;
typedef struct ASTNodeLoopInf     ASTNodeLoopInf;
typedef struct ASTNodeLoopForeach ASTNodeLoopForeach;
typedef struct ASTNodeFuncDef     ASTNodeFuncDef;
typedef struct ASTNodeFuncCall    ASTNodeFuncCall;
typedef struct ASTNodeReturn      ASTNodeReturn;
typedef struct ASTNodeTypeDecl    ASTNodeTypeDecl;
typedef struct ASTNodeTypeDef     ASTNodeTypeDef;
typedef struct ASTNodeNew         ASTNodeNew;
typedef struct ASTNodeError       ASTNodeError;
typedef struct ASTNodeDeal        ASTNodeDeal;

// represent the include statement:
// include <stdio.h>
// include "string.h"
// include <myfile.cplus>
struct ASTNodeInclude {
    int32           pos_line;
    char*           file;
    ASTNodeInclude* next;
};

// represent the module statement:
// module fmt
// module os/user
// module net/http
struct ASTNodeModule {
    int32          pos_line;
    char*          module;
    ASTNodeModule* next;
};

// represent any type statement.
struct ASTNode {
    int8 node_type;
    union {
        ASTNodeBlock*       node_block;
        ASTNodeExpr*        node_expr;
        ASTNodeDecl*        node_decl;
        ASTNodeAssign*      node_assign;
        ASTNodeIf*          node_if;
        ASTNodeSwitch*      node_switch;
        ASTNodeLoopFor*     node_loop_for;
        ASTNodeLoopWhile*   node_loop_while;
        ASTNodeLoopInf*     node_loop_inf;
        ASTNodeLoopForeach* node_loop_foreach;
        ASTNodeReturn*      node_return;
    }node;
};

// represent an statement in one of the Block/CaseBody/GlobalScope.
struct ASTNodeStmt {
    ASTNode*     stmt;
    ASTNodeStmt* next;
};

// represent a set of statements between a couple of braces('{' and '}').
struct ASTNodeBlock {
    ASTNodeStmt* stmts;
};

// represent a set of statements in an keyword case's scope.
struct ASTNodeCaseBody {
    ASTNodeStmt* stmts;
};

// represent a set of statements in global scope of the source file.
struct ASTNodeGlobalScope {
    ASTNodeModule*  modules;
    ASTNodeInclude* includes;
    ASTNodeStmt*    stmts;
};

struct ASTNodeID {
    int32 pos_line;
    int16 pos_col;
    char* id;
};

// represent the constant literal.
struct ASTNodeConstLit {
    int32 pos_line;
    int16 pos_col;
    // lit_type:
    //    TOKEN_CONST_INTEGER or
    //    TOKEN_CONST_FLOAT   or
    //    TOKEN_CONST_CHAR    or
    //    TOKEN_CONST_STRING
    int16 const_type;
    // lit_value:
    //    12, 5.21, "hello world", 'a'...
    char* const_value;
};

// represent an expression.
//
// example of the struct about the expression:
//                expr
//                 |
//            expr_binary
//         (oprd1,optr,oprd2)
//           /            \
//     expr_binary       expr_unary
// (oprd1,optr,oprd2)   (optr,oprd2)
//    |          |              |
//   expr       expr           expr
//    |          |              |
//  ident  const_integer     func_call
//
struct ASTNodeExpr {
    int8 expr_type;
    union {
        ASTNodeID*       expr_id;
        ASTNodeConstLit* expr_const_lit;
        ASTNodeIndex*    expr_index;
        ASTNodeFuncDef*  expr_func_def;
        ASTNodeFuncCall* expr_func_call;
        ASTNodeTypeDef*  expr_type_def;
        ASTNodeNew*      expr_new;
        ASTNodeExprUnry* expr_unary;
        ASTNodeExprBnry* expr_binary;
    }expr;
};

typedef struct ASTNodeExprListNode {
    ASTNodeExpr* expr;
    struct ASTNodeExprListNode* next;
}ASTNodeExprListNode;

// represent a set of expressions separated by comma(',')
struct ASTNodeExprList {
    ASTNodeExprListNode* exprs;
};

// represent an unary expression.
struct ASTNodeExprUnry {
    int16        op_token_code;
    ASTNodeExpr* oprd;
};

// represent a binary expression.
struct ASTNodeExprBnry {
    int16        op_token_code;
    ASTNodeExpr* oprd1;
    ASTNodeExpr* oprd2;
};

// represent the operation about extracting data from the container
// (e.g. array, slice).
struct ASTNodeIndex {
    ASTNodeID*   array;
    ASTNodeExpr* index;
};

// represent the declaration statement.
struct ASTNodeDecl {
    ASTNodeExpr* decl_type;
    char*        decl_idname;
    ASTNodeExpr* decl_init;
};

// represent the assignment statement.
struct ASTNodeAssign {
    ASTNodeExpr* expr_lhs;
    ASTNodeExpr* expr_rhs;
};

// represent the if statement:
// if cond {...} ef cond {...} [ef] else {...}
struct ASTNodeIf {
    ASTNodeExpr*  cond;
    ASTNodeBlock* block;

    // the branch_ef is the list which contains a set of ef statement.
    ASTNodeEf*    branch_ef;
    ASTNodeElse*  branch_else;
};

// represent the ef statement.
struct ASTNodeEf {
    ASTNodeExpr*  cond;
    ASTNodeBlock* block;
    ASTNodeEf*    next;
};

// represent the else statement.
struct ASTNodeElse {
    ASTNodeBlock* block;
};

// represent the switch statement:
// switch option {
// case const_expr1: ...
// case const_expr2: ...
// default:
//     ...
// }
struct ASTNodeSwitch {
    ASTNodeExpr*       option;
    ASTNodeSwitchCase* branch_case;
    ASTNodeSwitchDeft* branch_default;
};

// represent the case statement of the switch statement.
struct ASTNodeSwitchCase {
    ASTNodeExpr*       value;
    ASTNodeCaseBody*   body;
    ASTNodeSwitchCase* next;
};

// represent the default statement of the switch statement.
struct ASTNodeSwitchDeft {
    ASTNodeBlock* block;
};

// represent the loop statement with for-style:
// for init; cond; step {
//     ...
// }
struct ASTNodeLoopFor {
    int8 init_type;
    union {
        ASTNodeAssign* init_assign;
        ASTNodeDecl*   init_decl;
    }init;

    ASTNodeExpr*  cond;
    ASTNodeExpr*  step;
    ASTNodeBlock* block;
};

// represent the loop statement with while-style:
// for cond {
//     ...
// }
struct ASTNodeLoopWhile {
    ASTNodeExpr*  cond;
    ASTNodeBlock* block;
};

// represent the loop statement with infinite-style:
// for {
//     ...
// }
struct ASTNodeLoopInf {
    ASTNodeBlock* block;
};

// represent the loop statement with foreach-style:
// for data, index : container {
//     ...
// }
struct ASTNodeLoopForeach {
    ASTNodeExpr*  data;
    ASTNodeExpr*  index;
    ASTNodeExpr*  container;
    ASTNodeBlock* block;
};

struct ASTNodeFuncDef {
    char* func_name;
};

struct ASTNodeFuncCall {
    ASTNodeID*       func_name;
    ASTNodeExprList* func_params;
};

typedef struct {
    ASTNodeGlobalScope* global_scope;
}AST;

extern void astInit   (AST* ast);
extern void astDisplay(AST* ast);
extern void astDestroy(AST* ast);

#endif
