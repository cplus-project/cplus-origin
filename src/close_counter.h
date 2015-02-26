/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     The close_counter.h and close_counter.c provide
 * a kind of stack to manage any operator which need to
 * be closed.
 **/

#ifndef CPLUS_CLOSE_COUNTER_H
#define CPLUS_CLOSE_COUNTER_H

#include <string.h>
#include "common.h"

typedef struct close_counter_node {
    char optr;
    struct close_counter_node* next;
}close_counter_node;

// when the close_counter meet the left-side operator
// like (, [, { or /*, it will push the operator in
// the stack.
// when the close_counter meet the right-sideoperator
// like ), ], } or */, it will pop the top one
// operator in the stack.
typedef struct {
    close_counter_node* top;
}close_counter;

extern void  close_counter_init    (close_counter* clsctr);
extern void  close_counter_increase(close_counter* clsctr, char left_optr);
extern error close_counter_decrease(close_counter* clsctr, char right_optr);
extern bool  close_counter_is_clear(close_counter* clsctr);
extern void  close_counter_destroy (close_counter* clsctr);

#endif
