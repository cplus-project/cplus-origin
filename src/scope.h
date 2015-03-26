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
#include "id.h"
#include "func.h"
#include "type.h"

typedef struct scope {
    id_table   idtab;
    type_table typetab;
    func_table functab;
    struct scope* outer;
}scope;

extern void  scope_init       (scope* scp, scope* outer);
extern error scope_insert_id  (scope* scp, id     idinfo);
extern error scope_update_id  (scope* scp, id     idinfo);
extern id*   scope_search_id  (scope* scp, char*  id_name);
extern error scope_insert_type(scope* scp, type   tp);
extern type* scope_search_type(scope* scp, char*  tp_name);
extern error scope_insert_func(scope* scp, func   fn);
extern func* scope_search_func(scope* scp, char*  fn_name);
extern void  scope_destroy    (scope* scp);

#endif
