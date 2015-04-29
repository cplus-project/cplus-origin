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
#include "ident.h"

// a scope saves a set of named object and manages their lifecycle.
typedef struct scope {
    ident_table itab;
    struct scope* outer;
}scope;

extern void   scope_init     (scope* scp, scope* outer);
extern error  scope_add_id   (scope* scp, ident* id);
extern ident* scope_search_id(scope* scp, char*  id_name);
extern void   scope_destroy  (scope* scp);

#endif
