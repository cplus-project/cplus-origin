/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "scope.h"

void scope_init(scope* scp, scope* outer) {
    scp->outer = outer;
    id_table_init(&scp->idtab);
    func_table_init(&scp->functab);
    type_table_init(&scp->typetab);
}

error scope_insert_id(scope* scp, id idinfo) {
    return id_table_add(&scp->idtab, idinfo);
}

// update an id. if there are some ids with the same id name from
// different scope, it will update the one in the innermost scope.
error scope_update_id(scope* scp, id idinfo) {
    scope* ptr = scp;
    error  err = NULL;
    for (;;) {
        if (ptr == NULL) {
            return new_error("err: not found the id in all scope and can not update the it.");
        }
        err = id_table_update(&scp->idtab, idinfo);
        if (err == NULL) {
            return NULL;
        }
        ptr = ptr->outer;
    }
}

// search an id from the innermost scope to the outermost scope.
// return:
//       NULL -> not found the detail of the id
//   NOT NULL -> get the detail of the specific id
id* scope_search_id(scope* scp, char* id_name) {
    scope* ptr = scp;
    id*    ret = NULL;
    for (;;) {
        if (ptr == NULL) {
            return NULL;
        }
        ret = id_table_search(&ptr->idtab, id_name);
        if (ret != NULL) {
            return ret;
        }
        ptr = ptr->outer;
    }
}

error scope_insert_type(scope* scp, type tp) {
    return type_table_add(&scp->typetab, tp);
}

type* scope_search_type(scope* scp, char* tp_name) {
    return type_table_search(&scp->typetab, tp_name);
}

error scope_insert_func(scope* scp, func fn) {
    return func_table_add(&scp->functab, fn);
}

func* scope_search_func(scope* scp, char* fn_name) {
    return func_table_search(&scp->functab, fn_name);
}

void scope_destroy(scope* scp) {
    scp->outer = NULL;
    id_table_destroy(&scp->idtab);
    func_table_destroy(&scp->functab);
    type_table_destroy(&scp->typetab);
}
