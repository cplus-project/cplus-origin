/**
 * Copyright 2015 JiKai. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 **/

#include "scope.h"

void scope_init(scope* scp, scope* outer) {
    scp->outer = outer;
    idtable_init(&scp->idt);
    func_table_init(&scp->functab);
    type_table_init(&scp->typetab);
}

error scope_insert_id(scope* scp, id_info id) {
    return idtable_insert(&scp->idt, id);
}

// update an id. if there are some id with the same id name,
// it will update the one in the innermost scope.
error scope_update_id(scope* scp, id_info new_info) {
    scope* ptr = scp;
    error  err = NULL;
    for (;;) {
        if (ptr == NULL) {
            return new_error("err: not found the id in all scope.");
        }
        err = idtable_update(&ptr->idt, new_info);
        if (err == NULL) {
            return NULL;
        }
        ptr = ptr->outer;
    }
}

// search an id from the innermost scope to the outermost
// scope. the search result will be saved in the parameter
// 'ret'. if there is any error occur, an error will be
// returned.
error scope_search_id(scope* scp, id_info* search) {
    scope* ptr = scp;
    error  err = NULL;
    for (;;) {
        if (ptr == NULL) {
            return new_error("err: not found the id in all scope.");
        }
        err = idtable_search(&ptr->idt, search);
        if (err == NULL) {
            return NULL;
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
    idtable_destroy(&scp->idt);
    func_table_destroy(&scp->functab);
    type_table_destroy(&scp->typetab);
}

void scope_debug(scope* scp) {
    if (scp->outer != NULL) {
        debug("has parent scope");
    }
    else {
        debug("not has parent scope");
    }
    idtable_debug(&scp->idt);
}
