/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     The expression.h and expression.c provide some
 * structs and functions to assist to parse expression.
 **/

#ifndef CPLUS_EXPRESSION_H
#define CPLUS_EXPRESSION_H

#include "common.h"
#include "lexer.h"
#include "ast.h"

// the priority with a smaller number has the higher precedence.
#define OP_PRIORITY_NULL -1
#define OP_PRIORITY_0     9
#define OP_PRIORITY_1     8
#define OP_PRIORITY_2     7
#define OP_PRIORITY_3     6
#define OP_PRIORITY_4     5
#define OP_PRIORITY_5     4
#define OP_PRIORITY_6     3
#define OP_PRIORITY_7     2
#define OP_PRIORITY_8     1
#define OP_PRIORITY_9     0

extern int8 getOptrPriority(int16 op_token_code);

#define OP_TYPE_LUNARY   EXTRA_INFO_OP_LUNARY
#define OP_TYPE_RUNARY   EXTRA_INFO_OP_RUNARY
#define OP_TYPE_BINARY   EXTRA_INFO_OP_BINARY
#define OP_TYPE_EXPR_END EXTRA_INFO_EXPR_END
typedef struct OptrInfo {
    int16 op_token_code; // TOKEN_OP_XXX
    int8  op_type;       // OP_TYPE_LUNARY or OP_TYPE_RUNARY or OP_TYPE_BINARY
    int8  op_priority;   // OP_PRIORITY_0 to OP_PRIORITY_9
}OptrInfo;

typedef struct OptrStackNode {
    OptrInfo op;
    struct OptrStackNode* next;
}OptrStackNode;

// the OptrStack is used to save a set of operators' information
// to assist to parse the expression.
typedef struct {
    OptrStackNode* top;
}OptrStack;

extern void      optrStackInit   (OptrStack* optrstk);
extern void      optrStackPush   (OptrStack* optrstk, OptrInfo op);
extern bool      optrStackIsEmpty(OptrStack* optrstk);
extern OptrInfo* optrStackTop    (OptrStack* optrstk);
extern void      optrStackPop    (OptrStack* optrstk);
extern void      optrStackDestroy(OptrStack* optrstk);

typedef struct OprdStackNode {
    ASTNodeExpr* oprd;
    struct OprdStackNode* next;
}OprdStackNode;

// the OprdStack is used to save a set of ASTNodeExpr to assist
// to parse the expression.
typedef struct {
    OprdStackNode* top;
    int            oprd_count;
}OprdStack;

extern void         oprdStackInit     (OprdStack* oprdstk);
extern void         oprdStackPush     (OprdStack* oprdstk, ASTNodeExpr* oprdexpr);
extern bool         oprdStackIsEmpty  (OprdStack* oprdstk);
extern ASTNodeExpr* oprdStackTop      (OprdStack* oprdstk);
extern void         oprdStackPop      (OprdStack* oprdstk);
extern error        oprdStackCalcuOnce(OprdStack* oprdstk, OptrInfo   op);
extern error        oprdStackCalcu    (OprdStack* oprdstk, OptrStack* optrstk);
extern ASTNodeExpr* oprdStackGetResult(OprdStack* oprdstk);
extern void         oprdStackDestroy  (OprdStack* oprdstk);

#endif
