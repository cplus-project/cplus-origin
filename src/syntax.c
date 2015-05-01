/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "syntax.h"

/****** methods of operator_stack ******/

void operator_stack_init(operator_stack* optrstk) {
    optrstk->top = NULL;
}

void operator_stack_push(operator_stack* optrstk, int16 op_token_code) {
    operator_stack_node* create = (operator_stack_node*)mem_alloc(sizeof(operator_stack_node));
    create->op_token_code = op_token_code;
    create->next          = NULL;
    if (optrstk->top != NULL) {
        create->next = optrstk->top;
        optrstk->top = create;
    }
    else {
        optrstk->top = create;
    }
}

// return true if the stack is empty.
bool operator_stack_isempty(operator_stack* optrstk) {
    if (optrstk->top == NULL) {
        return true;
    }
    return false;
}

int16 operator_stack_top(operator_stack* optrstk) {
    return optrstk->top->op_token_code;
}

void operator_stack_pop(operator_stack* optrstk) {
    operator_stack_node* temp = optrstk->top;
    optrstk->top = optrstk->top->next;
    mem_free(temp);
}

void operator_stack_destroy(operator_stack* optrstk) {
    operator_stack_node* temp;
    for (;;) {
        if (optrstk->top == NULL) {
            return;
        }
        temp = optrstk->top;
        optrstk->top = optrstk->top->next;
        mem_free(temp);
    }
}
