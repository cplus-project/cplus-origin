/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "close_counter.h"

void close_counter_init(close_counter* clsctr) {
    clsctr->top = NULL;
}

void close_counter_increase(close_counter* clsctr, char left_optr) {
    close_counter_node* create = (close_counter_node*)mem_alloc(sizeof(close_counter_node));
    create->optr = left_optr;
    if (clsctr->top != NULL) {
        create->next = clsctr->top;
        clsctr->top  = create;
    }
    else {
        clsctr->top = create;
    }
}

error close_counter_decrease(close_counter* clsctr, char right_optr) {
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
    close_counter_node* temp = clsctr->top;
    clsctr->top = clsctr->top->next;
    mem_free(temp);
}

// return true when there is nothing in the stack.
bool close_counter_is_clear(close_counter* clsctr) {
    if (clsctr->top == NULL) {
        return true;
    }
    return false;
}

void close_counter_destroy(close_counter* clsctr) {
    if (clsctr->top == NULL) {
        return;
    }
    close_counter_node* temp = NULL;
    for (;;) {
        temp = clsctr->top;
        if (temp == NULL) {
            return;
        }
        clsctr->top = clsctr->top->next;
        mem_free(temp);
    }
}

void close_counter_debug(close_counter* clsctr) {
    printf("all nodes in the close_counter is:\r\n");
    if (clsctr->top == NULL) {
        debug("no any node");
    }
    else {
        close_counter_node* ptr = clsctr->top->next;
        printf("| %c |<-top\r\n", clsctr->top->optr);
        for (;;) {
            if (ptr == NULL) {
                return;
            }
            printf("| %c |\r\n", ptr->optr);
            ptr = ptr->next;
        }
    }
}
