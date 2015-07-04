/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "expression.h"

int8 getOptrPriority(int16 op_token_code) {
    if (op_token_code > 408) {
        if (op_token_code < 431) {
            if (op_token_code < 414) return OP_PRIORITY_0;
            if (op_token_code < 421) return OP_PRIORITY_1;
            if (op_token_code < 424) return OP_PRIORITY_2;
            if (op_token_code < 426) return OP_PRIORITY_3;
            if (op_token_code < 428) return OP_PRIORITY_4;
            return OP_PRIORITY_5;
        }
        if (op_token_code < 432) return OP_PRIORITY_6;
        if (op_token_code < 438) return OP_PRIORITY_7;
        if (op_token_code < 439) return OP_PRIORITY_8;
        if (op_token_code < 440) return OP_PRIORITY_9;
    }
    return OP_PRIORITY_NULL;
}

/****** methods of OptrStack ******/

void optrStackInit(OptrStack* optrstk) {
    optrstk->top = NULL;
}

void optrStackPush(OptrStack* optrstk, OptrInfo op) {
    OptrStackNode* create = (OptrStackNode*)mem_alloc(sizeof(OptrStackNode));
    create->op   = op;
    create->next = NULL;
    if (optrstk->top != NULL) {
        create->next = optrstk->top;
        optrstk->top = create;
    }
    else {
        optrstk->top = create;
    }
}

// return true if the stack is empty.
bool optrStackIsEmpty(OptrStack* optrstk) {
    if (optrstk->top == NULL) {
        return true;
    }
    return false;
}

OptrInfo* optrStackTop(OptrStack* optrstk) {
    return &optrstk->top->op;
}

void optrStackPop(OptrStack* optrstk) {
    OptrStackNode* temp = optrstk->top;
    optrstk->top = optrstk->top->next;
    mem_free(temp);
}

void optrStackDestroy(OptrStack* optrstk) {
    OptrStackNode* temp;
    for (;;) {
        if (optrstk->top == NULL) {
            return;
        }
        temp = optrstk->top;
        optrstk->top = optrstk->top->next;
        mem_free(temp);
    }
}

/****** methods of OprdStack ******/

void oprdStackInit(OprdStack* oprdstk) {
    oprdstk->top        = NULL;
    oprdstk->oprd_count = 0;
}

void oprdStackPush(OprdStack* oprdstk, ASTNodeExpr* oprdexpr) {
    OprdStackNode* create = (OprdStackNode*)mem_alloc(sizeof(OprdStackNode));
    create->oprd = oprdexpr;
    create->next = NULL;
    if (oprdstk->top != NULL) {
        create->next = oprdstk->top;
        oprdstk->top = create;
    }
    else {
        oprdstk->top = create;
    }
    oprdstk->oprd_count++;
}

// return true if the stack is empty.
bool oprdStackIsEmpty(OprdStack* oprdstk) {
    if (oprdstk->top == NULL) {
        return true;
    }
    return false;
}

ASTNodeExpr* oprdStackTop(OprdStack* oprdstk) {
    return oprdstk->top->oprd;
}

void oprdStackPop(OprdStack* oprdstk) {
    oprdstk->top = oprdstk->top->next;
    oprdstk->oprd_count--;
}

error oprdStackCalcuOnce(OprdStack* oprdstk, OptrInfo op) {
    switch (op.op_type) {
    case OP_TYPE_LUNARY:
    case OP_TYPE_RUNARY: {
            // get the operand of the unary operator
            if (oprdStackIsEmpty(oprdstk) == true) {
                return new_error("no enough operands.");
            }
            ASTNodeExpr* oprd = oprdStackTop(oprdstk);
            oprdStackPop(oprdstk);

            // get the result expression
            ASTNodeExpr* calcu_ret = (ASTNodeExpr*)mem_alloc(sizeof(ASTNodeExpr*));
            calcu_ret->expr_type = AST_NODE_EXPR_UNRY;
            calcu_ret->expr.expr_unary = (ASTNodeExprUnry*)mem_alloc(sizeof(ASTNodeExprUnry));
            calcu_ret->expr.expr_unary->op_token_code = op.op_token_code;
            calcu_ret->expr.expr_unary->oprd = oprd;
            oprdStackPush(oprdstk, calcu_ret);
        }
        break;

    case OP_TYPE_BINARY: {
            // get the two operands of the binary operator
            if (oprdStackIsEmpty(oprdstk) == true) {
                return new_error("no enough operands.");
            }
            ASTNodeExpr* oprd2 = oprdStackTop(oprdstk);
            oprdStackPop(oprdstk);
            if (oprdStackIsEmpty(oprdstk) == true) {
                return new_error("no enough operands.");
            }
            ASTNodeExpr* oprd1 = oprdStackTop(oprdstk);
            oprdStackPop(oprdstk);

            // get the result expression
            ASTNodeExpr* calcu_ret = (ASTNodeExpr*)mem_alloc(sizeof(ASTNodeExpr));
            calcu_ret->expr_type = AST_NODE_EXPR_BNRY;
            calcu_ret->expr.expr_binary = (ASTNodeExprBnry*)mem_alloc(sizeof(ASTNodeExprBnry));
            calcu_ret->expr.expr_binary->op_token_code = op.op_token_code;
            calcu_ret->expr.expr_binary->oprd1 = oprd1;
            calcu_ret->expr.expr_binary->oprd2 = oprd2;
            oprdStackPush(oprdstk, calcu_ret);
        }
        break;
    }

    return NULL;
}

error oprdStackCalcu(OprdStack* oprdstk, OptrStack* optrstk) {
    error     err;
    OptrInfo* top_optr;
    for (;;) {
        top_optr = optrStackIsEmpty(optrstk) == false ? (optrStackTop(optrstk)) : NULL;
        if (top_optr == NULL || top_optr->op_token_code == TOKEN_OP_LPARENTHESE) {
            return NULL;
        }
        if ((err = oprdStackCalcuOnce(oprdstk, *top_optr)) != NULL) {
            return err;
        }
    }
}

ASTNodeExpr* oprdStackGetResult(OprdStack* oprdstk) {
    if (oprdstk->oprd_count == 1)
        return oprdStackTop(oprdstk);
    else
        return (void*)-1;
}

void oprdStackDestroy(OprdStack* oprdstk) {
    OprdStackNode* temp;
    for (;;) {
        if (oprdstk->top == NULL) {
            return;
        }
        temp = oprdstk->top;
        oprdstk->top = oprdstk->top->next;
        mem_free(temp);
    }
}