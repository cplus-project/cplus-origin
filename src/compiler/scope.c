/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "scope.h"

void scopeInit(Scope* scope, Scope* outer) {
    identTableInit(&scope->id_table);
    scope->outer = outer;
}

error scopeAddID(Scope* scope, Ident* id) {
    return identTableAdd(&scope->id_table, id);
}

// search an identifier from the outermost scope to the innermost
// scope. return NULL if the identify is not in all scopes.
Ident* scopeSearchID(Scope* scope, char* id_name) {
    Ident* id;
    Scope* ptr = scope;
    for (;;) {
        id = identTableSearch(&ptr->id_table, id_name);
        if (id != NULL) {
            return id;
        }
        ptr = ptr->outer;
        if (ptr == NULL) {
            return NULL;
        }
    }
}

void scopeDestroy(Scope* scope) {
    identTableDestroy(&scope->id_table);
    scope->outer = NULL;
}
