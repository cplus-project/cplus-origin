/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "expression.h"

int8 get_op_priority(int16 op_token_code) {
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

/****** methods of optr_stack ******/

void optr_stack_init(optr_stack* optrstk) {
    optrstk->top = NULL;
}

void optr_stack_push(optr_stack* optrstk, optr op) {
    optr_stack_node* create = (optr_stack_node*)mem_alloc(sizeof(optr_stack_node));
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
bool optr_stack_isempty(optr_stack* optrstk) {
    if (optrstk->top == NULL) {
        return true;
    }
    return false;
}

optr* optr_stack_top(optr_stack* optrstk) {
    return &optrstk->top->op;
}

void optr_stack_pop(optr_stack* optrstk) {
    optr_stack_node* temp = optrstk->top;
    optrstk->top = optrstk->top->next;
    mem_free(temp);
}

void optr_stack_destroy(optr_stack* optrstk) {
    optr_stack_node* temp;
    for (;;) {
        if (optrstk->top == NULL) {
            return;
        }
        temp = optrstk->top;
        optrstk->top = optrstk->top->next;
        mem_free(temp);
    }
}

/****** methods of oprd_stack ******/

void oprd_stack_init(oprd_stack* oprdstk) {
    oprdstk->top = NULL;
}

void oprd_stack_push(oprd_stack* oprdstk, smt_expr* oprdexpr) {
    oprd_stack_node* create = (oprd_stack_node*)mem_alloc(sizeof(oprd_stack_node));
    create->oprd = oprdexpr;
    create->next = NULL;
    if (oprdstk->top != NULL) {
        create->next = oprdstk->top;
        oprdstk->top = create;
    }
    else {
        oprdstk->top = create;
    }
}

// return true if the stack is empty.
bool oprd_stack_isempty(oprd_stack* oprdstk) {
    if (oprdstk->top == NULL) {
        return true;
    }
    return false;
}

smt_expr* oprd_stack_top(oprd_stack* oprdstk) {
    return oprdstk->top->oprd;
}

void oprd_stack_pop(oprd_stack* oprdstk) {
    oprdstk->top = oprdstk->top->next;
}

error oprd_stack_calcu_once(oprd_stack* oprdstk, optr op) {
    switch (op.op_type) {
    case OP_TYPE_LUNARY:
    case OP_TYPE_RUNARY: {
            // get the operand of the unary operator
            if (oprd_stack_isempty(oprdstk) == true) {
                return new_error("no enough operands.");
            }
            smt_expr* oprd = oprd_stack_top(oprdstk);
            oprd_stack_pop(oprdstk);
            
            // get the result expression
            smt_expr* calcu_ret = (smt_expr*)mem_alloc(sizeof(smt_expr*));
            calcu_ret->expr_type = SMT_EXPR_UNARY;
            calcu_ret->expr.expr_unary = (smt_expr_unary*)mem_alloc(sizeof(smt_expr_unary));
            calcu_ret->expr.expr_unary->op_token_code = op.op_token_code;
            calcu_ret->expr.expr_unary->oprd          = oprd;
            oprd_stack_push(oprdstk, calcu_ret);
        }
        break;
    case OP_TYPE_BINARY: {
            // get the two operands of the binary operator
            if (oprd_stack_isempty(oprdstk) == true) {
                return new_error("no enough operands.");
            }
            smt_expr* oprd2 = oprd_stack_top(oprdstk);
            oprd_stack_pop(oprdstk);
            if (oprd_stack_isempty(oprdstk) == true) {
                return new_error("no enough operands.");
            }
            smt_expr* oprd1 = oprd_stack_top(oprdstk);
            oprd_stack_pop(oprdstk);

            // get the result expression
            smt_expr* calcu_ret = (smt_expr*)mem_alloc(sizeof(smt_expr));
            calcu_ret->expr_type = SMT_EXPR_BINARY;
            calcu_ret->expr.expr_binary = (smt_expr_binary*)mem_alloc(sizeof(smt_expr_binary));
            calcu_ret->expr.expr_binary->op_token_code = op.op_token_code;
            calcu_ret->expr.expr_binary->oprd1 = oprd1;
            calcu_ret->expr.expr_binary->oprd2 = oprd2;
            oprd_stack_push(oprdstk, calcu_ret);
        }
        break;
    default:
        return new_error("invalid operator type.");
    }
    
    return NULL;
}

void oprd_stack_destroy(oprd_stack* oprdstk) {
    oprd_stack_node* temp;
    for (;;) {
        if (oprdstk->top == NULL) {
            return;
        }
        temp = oprdstk->top;
        oprdstk->top = oprdstk->top->next;
        mem_free(temp);
    }
}