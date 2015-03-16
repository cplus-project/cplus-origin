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

typedef struct scope {
    idtable idt;
    struct scope* outer;
}scope;

extern void  scope_init     (scope* scp, scope* outer);
extern error scope_insert_id(scope* scp, id_info  id);
extern error scope_update_id(scope* scp, id_info  new_info);
extern error scope_search_id(scope* scp, id_info* search);
extern void  scope_destroy  (scope* scp);
extern void  scope_debug    (scope* scp);

#endif
