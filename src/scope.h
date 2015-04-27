/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     The scope.h and scope.c provide the supports
 * of scope management.
 **/

#ifndef CPLUS_SCOPE_H
#define CPLUS_SCOPE_H

#include "common.h"

typedef struct scope {
    struct scope* outer;
}scope;

#endif
