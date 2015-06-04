/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     The closectr.h and closectr.c provide a kind of
 * stack to manage any operator which need to be closed.
 **/

#ifndef CPLUS_CLOSE_COUNTER_H
#define CPLUS_CLOSE_COUNTER_H

#include <string.h>
#include "common.h"

typedef struct CloseCounterNode {
    char optr;
    struct CloseCounterNode* next;
}CloseCounterNode;

// when the close counter meet the left-side operator
// like (, [, { or /*, it will push the operator in
// the stack.
// when the close counter meet the right-sideoperator
// like ), ], } or */, it will pop the top one
// operator in the stack.
typedef struct {
    CloseCounterNode* top;
}CloseCounter;

extern void  closeCounterInit    (CloseCounter* clsctr);
extern void  closeCounterIncrease(CloseCounter* clsctr, char left_optr);
extern error closeCounterDecrease(CloseCounter* clsctr, char right_optr);
extern bool  closeCounterIsClear (CloseCounter* clsctr);
extern void  closeCounterDestroy (CloseCounter* clsctr);

#endif
