/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "closectr.h"

void closeCounterInit(CloseCounter* clsctr) {
    clsctr->top = NULL;
}

void closeCounterIncrease(CloseCounter* clsctr, char left_optr) {
    CloseCounterNode* create = (CloseCounterNode*)mem_alloc(sizeof(CloseCounterNode));
    create->optr = left_optr;
    if (clsctr->top != NULL) {
        create->next = clsctr->top;
        clsctr->top  = create;
    }
    else {
        clsctr->top = create;
    }
}

error closeCounterDecrease(CloseCounter* clsctr, char right_optr) {
    if (clsctr->top == NULL) {
        return new_error("err: the close_counter is empty.");
    }
    // can only process the closing for (), [] or {}.
    switch (right_optr) {
    case ')':
        if (clsctr->top->optr != '(') {
            return new_error("err: miss match for the left opeator to close.");
        }
        break;
    case ']':
        if (clsctr->top->optr != '[') {
            return new_error("err: miss match for the left opeator to close.");
        }
        break;
    case '}':
        if (clsctr->top->optr != '{') {
            return new_error("err: miss match for the left opeator to close.");
        }
        break;
    default:
        return new_error("err: can only count the (), [] or {}.");
    }
    // pop the top node in the stack and release
    // the memory of the node.
    CloseCounterNode* temp = clsctr->top;
    clsctr->top = clsctr->top->next;
    mem_free(temp);
}

// return true when there is nothing in the stack.
bool closeCounterIsClear(CloseCounter* clsctr) {
    if (clsctr->top == NULL) {
        return true;
    }
    return false;
}

void closeCounterDestroy(CloseCounter* clsctr) {
    if (clsctr->top == NULL) {
        return;
    }
    CloseCounterNode* temp = NULL;
    for (;;) {
        temp = clsctr->top;
        if (temp == NULL) {
            return;
        }
        clsctr->top = clsctr->top->next;
        mem_free(temp);
    }
}
