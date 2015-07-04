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
typedef struct Scope {
    IdentTable id_table;
    struct Scope* outer;
}Scope;

extern void   scopeInit    (Scope* scope, Scope* outer);
extern error  scopeAddID   (Scope* scope, Ident* id);
extern Ident* scopeSearchID(Scope* scope, char*  id_name);
extern void   scopeDestroy (Scope* scope);

#endif
