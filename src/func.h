/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 *
 *     The func.h and func.c implement a red black tree
 * to save all functions' definition and let programmers
 * to search them quickly.
 **/

#ifndef CPLUS_FUNC_TABLE_H
#define CPLUS_FUNC_TABLE_H

#include "common.h"
#include "id.h"

typedef struct func_table_node {
    char*     func_name;
    decl_list params_list;
    decl_list retval_list;
    // TODO: represent the function body...
}func_table_node;

typedef struct {
    func_table_node* root;
}func_table;

#endif
